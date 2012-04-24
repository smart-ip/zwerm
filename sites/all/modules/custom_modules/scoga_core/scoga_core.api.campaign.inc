<?php
/**
 * Created by JetBrains PhpStorm.
 * User: creynder
 * Date: 20/04/12
 * Time: 14:24
 * To change this template use File | Settings | File Templates.
 */


function scoga_get_campaign_vos_by_UUIDs( $campaign_UUIDs ){
    return scoga_get_output( $campaign_UUIDs, 'scoga_get_campaign_vo_by_UUID' );
}

function scoga_get_campaign_vo_by_UUID($campaign_UUID)
{
    $campaign_nid = scoga_get_eid_by_uuid( $campaign_UUID );
    if( $campaign_nid ){
        return scoga_get_campaign_vo_by_nid( $campaign_nid );
    }
}

function scoga_get_campaign_vos_by_nids( $campaign_nids ){
    return scoga_get_output( $campaign_nids, 'scoga_get_campaign_vo_by_nid' );
}

function scoga_get_campaign_vo_by_nid( $campaign_nid ){
    $campaign_node = node_load( $campaign_nid );
    if( $campaign_node ){
        $team_objs = scoga_get_team_vos_by_campaign_nid( $campaign_nid );
        $campaign_obj = scoga_create_campaign_vo( $campaign_node, $team_objs );
        $campaign_obj[ 'teams' ] = $team_objs;
        return $campaign_obj;
    }
}

function scoga_create_campaign_vo( $campaign_node ){
    //dd( $campaign_node, 'campaign node');
    $lang = LANGUAGE_NONE;
    $begin_time_str = $campaign_node->field_campaign_period[ $lang ][ 0 ][ 'value' ];
    $begin_time_tstamp = strtotime( $begin_time_str );
    $end_time_str = $campaign_node->field_campaign_period[ $lang ][ 0 ][ 'value2' ];
    $end_time_tstamp = strtotime( $end_time_str );
    $output = array(
        'nid'               => $campaign_node->nid,
        'title'             => $campaign_node->title,
        'uuid'              => $campaign_node->uuid,
        'begin_date'        => $begin_time_tstamp,
        'end_date'          => $end_time_tstamp,
        'teams'             => NULL,
        'last_retrieved'    => time(),
    );
    return $output;
}



function scoga_get_team_vos_by_campaign_nid( $campaign_nid ){
    $query = new EntityFieldQuery();
    $entity_type = 'node';
    $result = $query
        ->entityCondition( 'entity_type', $entity_type )
        ->entityCondition( 'bundle', SCOGA_CAMPAIGNTEAM_NODETYPE )
        ->fieldCondition( 'field_campaignteam_campaign', 'target_id', $campaign_nid, '=' )
        ->execute();
    $campaignteam_nids = array_keys($result[ $entity_type ]);
    $team_objs = array();
    foreach( $campaignteam_nids as $campaignteam_nid ){
        $campaignteam_node = node_load( $campaignteam_nid );
        $lang = $campaignteam_node->language;
        $team_nid = $campaignteam_node->field_campaignteam_team[ $lang ][ 0 ][ 'target_id' ];
        $team_obj = scoga_get_team_vo_by_nid( $team_nid );
        $team_obj[ 'total_points' ] = $campaignteam_node->field_campaignteam_totalpoints[ $lang ][ 0 ][ 'value' ];
        $team_obj[ 'points_modifier' ] = $campaignteam_node->field_campaignteam_modifier[ $lang ][ 0 ][ 'value' ];
        $team_objs[] = $team_obj;
    }
    return $team_objs;
}

function scoga_get_team_nids_by_campaign_nid( $campaign_nid ){

    $query =
        "SELECT field_campaignteam_team_target_id "
        . "FROM {field_data_field_campaignteam_team} team "
        . "JOIN {field_data_field_campaignteam_campaign} campaign "
        . "ON team.entity_id = campaign.entity_id "
        . "WHERE campaign.field_campaignteam_campaign_target_id = :target_nid"
    ;
    $result = db_query( $query, array(':target_nid' => $campaign_nid) );
    $nids = array();
    foreach( $result as $row ){
        $nids[] = $row->field_campaignteam_team_target_id;
    }

    return $nids;
}
