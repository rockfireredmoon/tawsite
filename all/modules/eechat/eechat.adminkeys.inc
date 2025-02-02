<?php

require_once dirname(__FILE__) . '/eechat.common.inc';

function _get_games() {
    $games = array();
    if(user_access('access VAL')) {
        $games['VAL'] = 'Valkals Shadow';
    }
    if(user_access('access EETEST')) {
        $games['EETEST'] = 'Test Server';
    }
    return $games;
}

function check_server_access($user, $game, $check_exists = true) {
    if(!user_access('access ' . $game)) {
        form_set_error ( 'message', t ( 'You do not have access to the ' . $game . ' server.') );
        return false;
    }
    
    
    /* Check this user doesn't already have key for VAL */
    $query = db_select('{eechat}', 'n')
    ->condition('n.game', $game, '=')
    ->condition('n.uid', $user->uid, '=')
    ->fields('n', array('uid'))
    ->execute();
    if( ( $check_exists && $query->rowCount() > 0 ) || ( !$check_exists && $query->rowCount() == 0 )) {
        if(!user_access('multiple game accounts')) {
            form_set_error ( 'message', t ( 'You have access to the ' . $game . ' server, but are not allow to create multiple accounts.') );
            return false;
        }
    }
    
    return true;
}

function eechat_admin_link_account_submit($form, &$form_state) {
    global $user;
    
    $username = $form_state['values']['username'];
    $siteusername = $form_state['values']['siteusername'];
    $game = $form_state['values']['game'];
    
    if(!check_server_access($user, $game)) {
        return;
    } 

    // Get the UID for the username provided
    $theiruid = db_select('users', 'u')->fields('u',array('uid'))->condition('name', $siteusername)->execute()->fetchField();
    if( $theiruid < 1) {
        form_set_error ( 'message', t ( 'The web site username ' . $siteusername . ' does not exist.') );
        return;
    }
    
    // Check its not already linked
    $currentuid = db_select('eechat', 'u')->fields('u',array('uid'))->condition('username', $username)->execute()->fetchField();
    if( $currentuid > 0) {
        $currentusername = db_select('users', 'u')->fields('u',array('name'))->condition('uid', $currentuid)->execute()->fetchField();
        form_set_error ( 'message', t ( 'The game account ' . $username . ' is already linked to the website account ' . $currentusername . '.') );
        return;
    }
    
    $data = array (
        'authtoken' => urlencode ( variable_get('eechat_authtoken_' . $game, t('mysecret') ) ),
        'username' => urlencode ( $username ),
    );
    
    list ( $data, $request_headers ) = _doHTTP ( '/adminlinkaccount', $method = 'POST', $postdata = $data, $server = $game);
    
    if (isset ( $request_headers->http_code ) && $request_headers->http_code == 200) {
        if($data != "") {
            
            list($regkey, $grove) = explode(':',$data);
            db_insert('eechat')->fields(array(
                'uid' => $theiruid,
                'game' => $game,
                'username' => $username,
                'regkey' => $regkey,
                'grove' => $grove
            ))->execute();
            
            drupal_set_message ( "The game account for " . $username . " on " . $game . " has been linked to " . $siteusername . ".", $type = 'status' );
            $form_state['redirect'] = 'user';
        }
        else
            form_set_error ( 'message', t ( 'Failed to link game account. Please check the details you provided.') . " " . $data );
    } else {
        form_set_error ( 'message', t ( 'Failed to contact the game server. Please inform an administrator. The error code given was : ') . " " . $request_headers->http_code );
    }
}

function _is_valid_string($str) {
    for ($i = 0; $i < strlen($str); $i++){
        $code = ord($str[$i]);
        if($code <= 32 || $code > 127)
            return false;
        if($str[$i] == ';')
            return false;
        if($str[$i] == '&')
            return false;
        if($str[$i] == ',')
            return false;
        if($str[$i] == '|')
            return false;
    }
    return true;
}

function eechat_admin_link_account_form($form, &$form_state) {
    global $user;
    
    // Info
    $markup = "<div id=\"server-status\">\n";
    $markup = $markup . "<p class=\"helpbox server-description\">\n";
    $markup = $markup . "This form allows you as an administrator to link an existing game account " .
                        "to a website account, allowing the player to perform their own password " .
                        "resets and other account maintenance. This will soon be REQUIRD.";
    $markup = $markup . "<ul>";
    $markup = $markup . "<li>Enter the username the player uses to login to the <i>Earth Eterrnal</i> server and " .
                        "the username the player uses to login to this website.</li>";
    $markup = $markup . "<li>Click on <b>Link</b>. The details will be validated by the game server.</li>";
    $markup = $markup . "<li>Their old registration key will be retrieved from the server, and the key will now be visible in their <a href=\"/user\">Account</a> page.</li>";
    $markup = $markup . "<li>If you have access to more than one server, you will be able to chose it here. This will only be available if you a developer or tester.</li>";
    $markup = $markup . "</ul></p>\n";
    $markup = $markup . "<div class=\"fancy-separator\"></div>\n";
    $markup = $markup . "</div>\n";
        
    $form ['box_info'] = array (
        '#type' => 'markup',
        '#markup' => $markup
    );
    $form ['game' ] = array (
        '#type' => 'select',
        '#default_value' => 'VAL',
        '#options' => _get_games(),
        '#title' => 'Game Server',
        '#required' => TRUE
    );
    $form ['username' ] = array (
        '#type' => 'textfield',
        '#default_value' => $user->name,
        '#title' => 'Game Account User Name:',
        '#maxlength' => 32,
        '#size' => 15,
        '#required' => TRUE
    );
    $form ['siteusername' ] = array (
        '#type' => 'textfield',
        '#title' => 'Website Account User Name:',
        '#maxlength' => 32,
        '#size' => 15,
        '#default_value' => '',
        '#required' => TRUE
    );
    $form ['submit' ] = array (
        '#type' => 'submit',
        '#button_type' => 'big-green',
        '#value' => 'Link!',
        '#submit' => array('eechat_admin_link_account_submit')
    );
    
    return $form;
}

?>
