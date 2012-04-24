<?php
/**
 * Created by JetBrains PhpStorm.
 * User: creynder
 * Date: 20/04/12
 * Time: 14:24
 * To change this template use File | Settings | File Templates.
 */



function scoga_get_transaction_vos_by_period( $begin_time = NULL, $end_time = NULL ){

    if( $begin_time == NULL ){
        $begin_time = 0;
    }
    if( $end_time == NULL ){
        $end_time = time();
    }

    //dd( $begin_time, 'begin time');
    //dd( $end_time, 'end time' );

    $query  =
        " SELECT txn_id, time_stamp "
        . "FROM {userpoints_txn} "
        . "WHERE tid = :tid "
        . "AND time_stamp > :begin_time "
        . "AND time_stamp <= :end_time"
    ;
    $rows = db_query( $query, array(
        ':tid'          => SCOGA_SCORE_TID,
        ':begin_time'   => $begin_time,
        ':end_time'     => $end_time,
    ) );

    $options = array(
        'link' => FALSE,
        'truncate' => FALSE,
        'skip_description' => FALSE,
    );

    $result = array();
    foreach( $rows as $row ){
        $transaction = userpoints_transaction_load( $row->txn_id );
        $description = userpoints_create_description( $transaction, $options );
        $result[] = scoga_create_transaction_vo( $transaction, $description );
    }

    return $result;
}

function scoga_create_transaction_vo( $transaction, $description ){
    $output = array(
        'txn_id'        => $transaction->txn_id,
        'uid'           => $transaction->uid,
        'points'        => $transaction->points,
        'time_stamp'    => $transaction->time_stamp,
        'description'   => $description,
    );
    return $output;
}

