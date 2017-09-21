<?php

require_once dirname(__FILE__) . '/eechat.common.inc';

function _get_games() {
    $games = array();
    if(user_access('access SOA')) {
        $games['SOA'] = 'The Scourge Of Abidan';
    }
    if(user_access('access VAL')) {
        $games['VAL'] = 'Valkals Shadow';
    }
    return $games;
}

function check_server_access($user, $form, &$form_state) {
    $game = $form_state['values']['game'];
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
    if($query->rowCount() > 0) {
        form_set_error ( 'message', t ( 'You already have an account on this game server.') );
        return false;
    }
    
    return true;
}

function eechat_link_account_submit($form, &$form_state) {
    global $user;
    
    $username = $form_state['values']['username'];
    $password = $form_state['values']['password'];
    $game = $form_state['values']['game'];
    
    if(!check_server_access($user, $form, $form_state)) {
        return;
    }
    
    error_log('username ' . $username . " game " . $game);
    
    $data = array (
        'authtoken' => urlencode ( variable_get('eechat_authtoken_' . $game, t('mysecret') ) ),
        'username' => urlencode ( $username ),
        'password' => urlencode ( $password ),
    );
    
    list ( $data, $request_headers ) = _doHTTP ( '/linkaccount', $method = 'POST', $postdata = $data, $server = $game);
    
    if (isset ( $request_headers->http_code ) && $request_headers->http_code == 200) {
        if($data != "") {
            list($regkey, $grove) = explode(':',$data);
            db_insert('eechat')->fields(array(
                'uid' => $user->uid,
                'game' => $game,
                'username' => $username,
                'regkey' => $regkey,
                'grove' => $grove
            ))->execute();
            
            drupal_set_message ( "Your game account on " . $game . " has been linked.", $type = 'status' );
            $form_state['redirect'] = 'user';
        }
        else
            form_set_error ( 'message', t ( 'Failed to link game account. Please check the details you provided.') . " " . $data );
    } else {
        form_set_error ( 'message', t ( 'Failed to contact the game server. Please inform an administrator. The error code given was : ') . " " . $request_headers->http_code );
    }
}

function eechat_request_key_submit($form, &$form_state) {
    global $user;
    
    $regkey = bin2hex(openssl_random_pseudo_bytes(16));
    $username = $form_state['values']['username'];
    $password = $form_state['values']['password'];
    $grove = $form_state['values']['grove'];
    $game = $form_state['values']['game'];
    
    if(!check_server_access($user, $form, $form_state)) {
        return;
    }
    
    error_log('username ' . $username);
    
    $data = array (
        'authtoken' => urlencode ( variable_get('eechat_authtoken_' . $game, t('mysecret') ) ),
        'regkey' => urlencode ( $regkey ),
        'username' => urlencode ( $username ),
        'password' => urlencode ( $password ),
        'grove' => urlencode ( $grove )
    );
    
    list ( $data, $request_headers ) = _doHTTP ( '/newaccount', $method = 'POST', $postdata = $data, $server = $game);
    
    if (isset ( $request_headers->http_code ) && $request_headers->http_code == 200) {
        if($data == "Account creation was successful.") {
            
            db_insert('eechat')->fields(array(
                'uid' => $user->uid,
                'game' => $game,
                'username' => $username,
                'regkey' => $regkey,
                'grove' => $grove
            ))->execute();
            
            drupal_set_message ( "Your game account on " . $game . " has been created. Make sure you " . 
                                "have downloaded the latest client, start it up, and use the username and " .
                                "password you just provided to start character creation.", $type = 'status' );
            
            $form_state['redirect'] = 'user';
        }
        else
            form_set_error ( 'message', t ( 'Failed to create game account. Please check the details you provided.') . " " . $data );
    } else {
        form_set_error ( 'message', t ( 'Failed to contact the game server. Please inform an administrator. The error code given was : ') . " " . $request_headers->http_code );
    }
    
    //return $element;
}

function eechat_request_key_form($form, &$form_state) {
    global $user;
    
    // Info
    $markup = "<div id=\"server-status\">\n";
    $markup = $markup . "<p class=\"helpbox server-description\">\n";
    $markup = $markup . "You are requesting a key to play on the <i>Earth Eternal</i> ";
    $markup = $markup . "server. You must provide a <b>Username</b>, <b>Password</b> and <b>Default Grove Name</b> that will ";
    $markup = $markup . "be used to created your account.";
    $markup = $markup . "<ul>";
    $markup = $markup . "<li>Try to use the same username as you use on this site (i.e. <i>" . 
                        $user->name . "</i>). However, if this name is already taken by another player (you will receive an error telling you so), then instead pick another name.</li>";
                        $markup = $markup . "<li>If you already have an account on this game, you should instead use <a href='link-account'>Link Account</a> to link your <b>Game Account</b> to your <b>Website Account</b></li>";
    $markup = $markup . "<li>A unique <b>Key</b> will be generated for you. We store that on this site for you, but just in case, you should note this down somewhere else as it may be used to identity and retrieve your account in case of emergencies.</li>";
    $markup = $markup . "</ul></p>\n";
    $markup = $markup . "<div class=\"fancy-separator\"></div>\n";
    $markup = $markup . "</div>\n";
    
    $form ['box_info'] = array (
        '#type' => 'markup',
        '#markup' => $markup
    );
    $form ['game' ] = array (
        '#type' => 'select',
        '#default_value' => 'SOA',
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
    $form ['password' ] = array (
        '#type' => 'password',
        '#title' => 'Game Account Password:',
        '#maxlength' => 32,
        '#size' => 15,
        '#default_value' => '',
        '#required' => TRUE
    );
    $form ['grove' ] = array (
        '#type' => 'textfield',
        '#default_value' => $user->name,
        '#title' => 'Default Grove Name:',
        '#maxlength' => 15,
        '#size' => 15,
        '#required' => TRUE
    );
    $form ['submit' ] = array (
        '#type' => 'submit',
        '#value' => 'Request!',
        '#submit' => array('eechat_request_key_submit')
    );
    
    return $form;
}

function eechat_link_account_form($form, &$form_state) {
    global $user;
    
    // Info
    $markup = "<div id=\"server-status\">\n";
    $markup = $markup . "<p class=\"helpbox server-description\">\n";
    $markup = $markup . "You are chosing to link your game account  on the <i>Earth Eternal - Valkals Shadow</i> ";
    $markup = $markup . "server to this web server account. There are no immediate benefits to you doing this, but in the " .
                    "future, this will allow you to reset your game passwords and perform other account maintenance tasks." .
                    "It will also be used to aid migration to <i>The Anubian War</i> when it is released.";
    $markup = $markup . "<ul>";
    $markup = $markup . "<li>Enter the username and password you use to login to the <i>Valkals Shadow</i> server.</li>";
    $markup = $markup . "<li>Click on <b>Link</b>. Your username and password will be validated by the game server.</li>";
    $markup = $markup . "<li>Your registration key will be retrieved from the server, and the key will now be visible in your <a href=\"/user\">Account</a> page.</li>";
    $markup = $markup . "</ul></p>\n";
    $markup = $markup . "<div class=\"fancy-separator\"></div>\n";
    $markup = $markup . "</div>\n";
        
    $form ['box_info'] = array (
        '#type' => 'markup',
        '#markup' => $markup
    );
    $form ['game' ] = array (
        '#type' => 'select',
        '#default_value' => 'SOA',
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
    $form ['password' ] = array (
        '#type' => 'password',
        '#title' => 'Game Account Password:',
        '#maxlength' => 32,
        '#size' => 15,
        '#default_value' => '',
        '#required' => TRUE
    );
    $form ['submit' ] = array (
        '#type' => 'submit',
        '#value' => 'Link!',
        '#submit' => array('eechat_link_account_submit')
    );
    
    return $form;
}

?>