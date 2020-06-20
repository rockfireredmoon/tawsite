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
            form_set_error ( 'message', t ( 'You do not have access to the ' . $game . ' server, and are not allow to create multiple accounts.') );
            return false;
        }
    }
    
    return true;
}

function eechat_link_account_submit($form, &$form_state) {
    global $user;
    
    $username = $form_state['values']['username'];
    $password = $form_state['values']['password'];
    $game = $form_state['values']['game'];
    
    if(!check_server_access($user, $game)) {
        return;
    }
    
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
    $confirm_password = $form_state['values']['confirm_password'];
    
    if(!check_server_access($user, $game)) {
        return;
    }
    
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

function eechat_request_key_form_validate($form, &$form_state) {
    
    $username = $form_state['values']['username'];
    $password = $form_state['values']['password'];
    $confirm_password = $form_state['values']['confirm_password'];
    
    if($password != $confirm_password) {
        form_set_error ( 'message', t ( 'Password and confirmed password do not match.' ));
        return;
    }
    
    if(strlen($password) < 3 || strlen($password) > 32) {
        form_set_error ( 'message', t ( 'Password must be between 3 and 32 characters in length.' ));
        return;
    }
    
    if(!_is_valid_string($password)) {
        form_set_error ( 'message', t ( 'Password may not contain Space, ",", "&", "|", or ";".' ));
        return;
    }
    
    if(strlen($username) < 3 || strlen($username) > 48) {
        form_set_error ( 'message', t ( 'Username must be between 3 and 48 characters in length.' ));
        return;
    }
    
    if(!_is_valid_string($username)) {
        form_set_error ( 'message', t ( 'Username may not contain Space, ",", "&", "|", or ";".' ));
        return;
    }
}

function eechat_request_key_form($form, &$form_state) {
    global $user;
    
    if ($user->uid == 0) {
      drupal_goto('user/login');
      return;
    }

    // Info
    $markup = "<div id=\"server-status\">\n";
    $markup = $markup . "<p class=\"helpbox server-description\">\n";
    $markup = $markup . "You are requesting a key to play on the <i>Earth Eternal</i> ";
    $markup = $markup . "server. You must provide a <b>Username</b>, <b>Password</b> and <b>Default Grove Name</b> that will ";
    $markup = $markup . "be used to created your account.";
    $markup = $markup . "<ul>";
    $markup = $markup . "<li>Try to use the same username as you use on this site (i.e. <i>" . 
        $user->name . "</i>). However, if this name is already taken by another player (you will receive an error telling you so), then instead pick another name.</li>";
    $markup = $markup . "<li>Both Username and Password can contain any character except for <b>Space</b>, <b>&quot;,&quot;</b>, <b>&quot;;&quot;</b>, <b>&quot;|&quot;</b> and <b>&quot;&amp;&quot;</b></li>";
    $markup = $markup . "<li>If you already have an account on this game, you should instead use <a href='link-account'>Link Account</a> to link your <b>Game Account</b> to your <b>Website Account</b></li>";
    $markup = $markup . "<li>A unique <b>Key</b> will be generated for you. We store that on this site for you, but just in case, you should note this down somewhere else as it may be used to identity and retrieve your account in case of emergencies.</li>";
    $markup = $markup . "<li>Your <b>Grove</b> is your own personal home in Earth Eternal. You can decorate your home with any props or creatures found in the game! Please note though, this is an advanced topic. See <a href=\"/gameguides/grovebuilding\">Building Your Grove</a> in the Game Guide.</li>";
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
    $form ['password' ] = array (
        '#type' => 'password',
        '#title' => 'Game Account Password:',
        '#maxlength' => 32,
        '#size' => 15,
        '#default_value' => '',
        '#required' => TRUE
    );
    $form ['confirm_password' ] = array (
        '#type' => 'password',
        '#title' => 'Confirm Game Account Password:',
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
        '#button_type' => 'big-green',
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
    $markup = $markup . "You are chosing to link your game account on an <i>Earth Eternal</i> ";
    $markup = $markup . "server to this web server account. This allows you to perform your own  " .
                    "password resets, and in the future will allow launching the game right from this site.";
    $markup = $markup . "<ul>";
    $markup = $markup . "<li>Enter the username and password you use to login to the <i>Earth Eterrnal</i> server.</li>";
    $markup = $markup . "<li>Click on <b>Link</b>. Your username and password will be validated by the game server.</li>";
    $markup = $markup . "<li>Your registration key will be retrieved from the server, and the key will now be visible in your <a href=\"/user\">Account</a> page.</li>";
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
        '#button_type' => 'big-green',
        '#value' => 'Link!',
        '#submit' => array('eechat_link_account_submit')
    );
    
    return $form;
}

function eechat_change_password_form_validate($form, &$form_state) {
    
    $password = $form_state['values']['password'];
    $confirm_password = $form_state['values']['confirm_password'];
    
    if($password != $confirm_password) {
        form_set_error ( 'message', t ( 'Password and confirmed password do not match.' ));
        return;
    }
    
    if(strlen($password) < 3 || strlen($password) > 32) {
        form_set_error ( 'message', t ( 'Password must be between 3 and 32 characters in length.' ));
        return;
    }
    
    if(!_is_valid_string($password)) {
        form_set_error ( 'message', t ( 'Password may not contain Space, ",", "&", "|", or ";".' ));
        return;
    }
}

function eechat_change_password_form_submit($form, &$form_state) {
    global $user;
    
    $request_parts = explode('/', $_SERVER['REQUEST_URI']);
    $regkey = array_pop($request_parts);
    $result = db_query('SELECT * FROM {eechat} WHERE regkey = ?', array(
        $regkey
    ));
    $row = $result->fetchObject();
    $game = $row->game;
    $username = $row->username;
    $password = $form_state['values']['password'];
    
    if($user->uid != $row->uid) {
        form_set_error ( 'message', t ( 'You can only set your own users password.' ));
        return;
    }
    
    if(!check_server_access($user, $game, false)) {
        return;
    }
    
    $data = array (
        'regkey' => urlencode ( $regkey ),
        'username' => urlencode ( $username ),
        'password' => urlencode ( $password ),
        'authtoken' => urlencode ( variable_get('eechat_authtoken_' . $game, t('mysecret') ) ),
    );
    
    list ( $data, $request_headers ) = _doHTTP ( '/resetpassword', $method = 'POST', $postdata = $data, $server = $game);
    
    if (isset ( $request_headers->http_code ) && $request_headers->http_code == 200) {
        if($data == "Password was successfully reset.") {
            drupal_set_message ( "Your password for " . $username . " on " . $game . " has been changed.", $type = 'status' );
            $form_state['redirect'] = 'user';
        }
        else
            form_set_error ( 'message', t ( 'Failed to change game password. Please check the details you provided.') . " " . $data );
    } else {
        form_set_error ( 'message', t ( 'Failed to change game password. Please inform an administrator. The error code given was : ') . " " . $request_headers->http_code );
    }
}

function eechat_change_password_form($form, &$form_state) {
    global $user;
    $request_parts = explode('/', $_SERVER['REQUEST_URI']);
    $regkey = array_pop($request_parts);
    $result = db_query('SELECT * FROM {eechat} WHERE regkey = ?', array(
        $regkey
    ));
    $row = $result->fetchObject();
    $game = $row->game;
    $username = $row->username;
    
    if($user->uid != $row->uid) {
        form_set_error ( 'message', t ( 'You can only set your own users password.' ));
        return;
    }
    
    // Info
    $markup = "<div id=\"server-status\">\n";
    $markup = $markup . "<p class=\"helpbox server-description\">\n";
    $markup = $markup . "This page allows you to change your <b>Game Password</b> on an <i>Earth Eternal</i> ";
    $markup = $markup . "server</p>\n";
    $markup = $markup . "<ul><li>Passwords can contain any character except for <b>Space</b>, <b>&quot;,&quot;</b>, <b>&quot;;&quot;</b>, <b>&quot;|&quot;</b> and <b>&quot;&amp;&quot;</b></li></ul>";
    $markup = $markup . "<div class=\"fancy-separator\"></div>\n";
    $markup = $markup . "</div>\n";
    
    // TODO
    //$regkey = "XX";
    
    $form ['box_info'] = array (
        '#type' => 'markup',
        '#markup' => $markup
    );
    $form ['game' ] = array (
        '#type' => 'item',
        '#markup' => $game,
        '#title' => 'Game Server',
    );
    $form ['regkey'] = array (
        '#type' => 'item',
        '#title' => 'Game Key:',
        '#markup' => $regkey,
    );
    $form ['username'] = array (
        '#type' => 'item',
        '#title' => 'Game Account Username:',
        '#markup' => $username,
    );
    $form ['password' ] = array (
        '#type' => 'password',
        '#title' => 'Game Account Password:',
        '#maxlength' => 32,
        '#size' => 15,
        '#default_value' => '',
        '#required' => TRUE
    );
    $form ['confirm_password' ] = array (
        '#type' => 'password',
        '#title' => 'Confirm Game Account Password:',
        '#maxlength' => 32,
        '#size' => 15,
        '#default_value' => '',
        '#required' => TRUE
    );
    $form ['submit' ] = array (
        '#type' => 'submit',
        '#button_type' => 'big-green',
        '#value' => 'Change!',
        '#submit' => array('eechat_change_password_form_submit')
    );
    
    return $form;
}

?>
