<?php
/**
 * Created by JetBrains PhpStorm.
 * User: creynder
 * Date: 13/12/11
 * Time: 13:34
 * To change this template use File | Settings | File Templates.
 */

function _reader_checkin_index( $params ){
    watchdog( '[SCOGA] reader checkin index', serialize($params));
    $view = views_get_view( 'checkins' );
   	$view->execute_display( 'default' );
   	$results = $view->result;
    return $results;
}

function _reader_checkin_create( $params ){
    watchdog('[SCOGA} reader checkin create', serialize( $params ) );

    $lang = LANGUAGE_NONE; //'en';

    /*
     * catch parameters
     */
    $card_id = $params[ 'cardID' ];
    $reader_id = $params[ 'readerID' ];
    $checkin_time = $params[ 'timestamp' ];

    $user_profile = _retrieve_user_profile( $card_id );

    if( $user_profile === FALSE ) return 'could not retrieve/create user';

    $reader_node = _retrieve_reader_node( $reader_id );

    if( $reader_node === FALSE ) return 'could not retrieve/create reader';

    /*
     * create checkin node
     */
    $checkin_node = new stdClass();
    $checkin_node->type = 'checkin';
    node_object_prepare( $checkin_node );

    //required fields
    $checkin_node->uid = $user_profile->uid;
    $checkin_node->title = $user_profile->name
        . ' checked in with '
        . $reader_node->title;

    $checkin_node->language = $lang;

    //user reference
    $checkin_node->field_user_reference[ $lang ][ 0 ] = array(
        'uid' => $user_profile->uid
    );

    //reader reference
    $checkin_node->field_reader_reference[ $lang ][ 0 ] = array(
        'nid' => $reader_node->nid
    );


    //checkin time, if any
    if( $checkin_time ){
        $checkin_node->title .= ' at ' . date( 'H:i:s d/m/Y', intval( $checkin_time / 1000 ) );
        $checkin_node->field_checkin_time[ $lang ][ 0 ] = array(
            'value' => $checkin_time
        );
    }

    //save the mofo node
    $checkin_node = node_submit( $checkin_node );
    node_save( $checkin_node );

    return $checkin_node;
}

/*
 * retrieve user profile by card ID
 */
function _retrieve_user_profile( $card_id ){
    $user_entity_type = 'user';
    $user_query = new EntityFieldQuery();

    $user_entities = $user_query
        ->entityCondition( 'entity_type', $user_entity_type )
        ->fieldCondition( 'field_card_id', 'value', $card_id, '=', 0 )
        ->execute();

    $user_uids = array_keys( $user_entities[ $user_entity_type ] );

    //return count( $user_uids );
    switch( count( $user_uids ) ){
        case 0 : {
            return _create_kliek_user( $card_id );
        }
        case 1 : {
            return user_load( $user_uids[ 0 ] );
        }
        default : {
            return FALSE;
        }
    }
}

function _create_kliek_user( $card_id ){
    $user_profile = new stdClass();
    $user_profile->name = 'kliek user '. $card_id;
    $user_profile->status = 1;
    $user_profile->mail = 'none';
    $user_profile->roles = array(
        2 => 'authenticated user',
        4 => 'kliek user'
    );
    $user_profile->field_card_id[ LANGUAGE_NONE ][ 0 ] = array(
        'value' => $card_id
    );

    return user_save( $user_profile );
}

/*
 * get reader node by RFID reader ID
 */
function _retrieve_reader_node( $reader_id ){
    $reader_entity_type = 'node';
    $reader_query = new EntityFieldQuery();

    $reader_entities = $reader_query
        ->entityCondition( 'entity_type', $reader_entity_type )
        ->fieldCondition( 'field_reader_id', 'value', $reader_id, '=', 0 )
        ->execute();

    $reader_nids = array_keys( $reader_entities[ $reader_entity_type ] );

    switch( count( $reader_nids ) ){
        case 0 : {
            return _create_rfid_reader( $reader_id );
        }
        case 1 : {
            return node_load( $reader_nids[ 0 ] );
        }
        default : {
            return FALSE;
        }
    }

}

function _create_rfid_reader( $reader_id ){
    $reader_node = new stdClass();
    $reader_node->type = 'rfid_reader';
    node_object_prepare( $reader_node );
    $reader_node->language = LANGUAGE_NONE;
    $reader_node->title = 'RFID reader ' . $reader_id;
    $reader_node->field_reader_id[ LANGUAGE_NONE ][ 0 ] = array(
        'value' => $reader_id
    );

    //contrary to user_save node_save does NOT return the node object
    //but $reader_node is passed by reference, so it will contain the new nid
    node_save( $reader_node );

    return $reader_node;
}