<?php

require_once dirname(__FILE__) . '/eechat.common.inc';


function eechat_server_start_main($form, &$form_state) {
	$element = $form ['box_main'];
	return $element;
}

function eechat_server_stop_main($form, &$form_state) {
	$element = $form ['box_main'];
	return $element;
}

function eechat_server_stop_dev($form, &$form_state) {
			form_set_error ( 'message', t ( 'You must create a character in-game to be able to chat from the website.' ) );
	$element = $form ['box_dev'];
	$element ['#markup'] = "<h1>DONE!</h1>";
	$_SESSION['stopping_dev'] = true;
	$_SESSION['starting_dev'] = false;
	return $element;
}

function eechat_server_start_dev($form, &$form_state) {
	$element = $form ['box_dev'];
	$_SESSION['stopping_dev'] = false;
	$_SESSION['starting_dev'] = true;
	return $element;
}

/*function eechat_server_submit_callback($form, $form_state) {
	$element = $form ['box'];
	$element = $form ['box'];
	if(strlen($form_state ['values'] ['message']) > 0) {
		$data = array (
				'msg' => urlencode ( $form_state ['values'] ['message'] ),
				'from' => urlencode ( $form_state ['values'] ['character'] ),
				'channel' => urlencode ( $form_state ['values'] ['channel'] )
		);
		list ( $data, $request_headers ) = _doHTTP ( "/chat", "POST", $data );

		$chat = json_decode ( $data );
		$markup = _render_chat ( $chat );
		$markup = str_replace ( "'", "\'", $markup );
		$element ['#markup'] = "<script type=\"text/javascript\">document.getElementById('block-eechat-eefullchat-chat').innerHTML = '<ul>" . $markup . "</ul>' ; document.getElementById('edit-message').value = ''; </script>";
	}
	$element ['#markup'] = "<h1>DONE!</h1>";
	return $element;
}
	*/

function eechat_server_form($form, &$form_state) {
	global $user;
	
	/*drupal_add_js ( drupal_get_path ( 'module', 'eechat' ) . '/js/eeleaderboard.js' );*/
	$urlp = "/up" ;
	list ( $data, $request_headers ) = _doHTTP ( $urlp );
	
	// Info
	$markup = "<div id=\"server-status\">\n";
	$markup = $markup . "<p class=\"helpbox server-description\">\n";
	$markup = $markup . "This page shows the current status of our servers.";
	$markup = $markup . "</p>\n";
	$markup = $markup . "<div class=\"fancy-separator\"></div>\n";
	$markup = $markup . "</div>\n";
	
	$form ['box_info'] = array (
			'#type' => 'markup',
			'#markup' => $markup
	);

	// Dev server
	$main_status = "down";
	$main_status_text = "Server not responding";
	
	if(isset($_SESSION['starting_dev']) && $_SESSION['starting_dev']) {
		$main_status = "starting";
		$main_status_text = "You have requested the server start. Please wait";
	}
	else if(isset($_SESSION['stopping_dev']) && $_SESSION['stopping_dev']) {
		$main_status = "stopping";
		$main_status_text = "You have requested the server stop. Please wait";
	}
	else if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$clansdata = json_decode ( $data );
		$main_status = "up";
		$main_status_text = "Server accepting requests";
	}
	else if (isset ( $request_headers->http_code ) && $request_headers->http_code != 0) {
		$main_status_text = "Server up, but not accepting requests. Response code " . $request_headers->http_code ;
	}
	
	$markup = "<div class=\"server-status main-server\">\n";
	$markup = $markup . "<h2>Developer Server (TAWD)</h2>";
	$markup = $markup . "<label class=\"status status-" . $main_status . "\">" . $main_status . "</label>";
	$markup = $markup . "<span class=\"status-detail\">" . $main_status_text . "</span>";
	$markup = $markup . "</div>\n";
	
	$form ['box_dev'] = array (
			'#type' => 'markup',
			'#prefix' => '<div class="server-status-box server-status-box-dev">',
			'#suffix' => '</div>',
			'#markup' => $markup
	);
	
	if($main_status === "up")
		$form ['stop_dev'] = array (
				'#type' => 'submit',
				'#value' => 'Stop1',
				'#submit' => array('eechat_server_stop_dev')
		);
	else if($main_status === "down")
		$form ['start_dev'] = array (
				'#type' => 'submit',
				'#value' => 'Start1',
				'#submit' => array('eechat_server_start_dev')
		);
	
	// Main server
	$main_status = "down";
	$main_status_text = "Server not responding";
	
	if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$clansdata = json_decode ( $data );
		$main_status = "up";
		$main_status_text = "Server accepting requests";
	}
	else if (isset ( $request_headers->http_code ) && $request_headers->http_code != 0) {
		$main_status_text = "Server up, but not accepting requests. Response code " . $request_headers->http_code ;
	}
	
	$markup = "<div class=\"server-status main-server\">\n";
	$markup = $markup . "<h2>Main Server (SOAD)</h2>";
	$markup = $markup . "<label class=\"status status-" . $main_status . "\">" . $main_status . "</label>";
	$markup = $markup . "<span class=\"status-detail\">" . $main_status_text . "</span>";
	$markup = $markup . "</div>\n";
	
	$form ['box_main'] = array (
			'#type' => 'markup',
			'#prefix' => '<div class="fancy-separator"></div><div class="server-status-box server-status-box-main">',
			'#suffix' => '</div>',
			'#markup' => $markup
	);	
	if($main_status === "up")
		$form ['stop_main'] = array (
				'#type' => 'submit',
				'#value' => 'Stop',
				'#submit' => array('eechat_server_stop_main')
		);
	else if($main_status === "down")
		$form ['start_main'] = array (
				'#type' => 'submit',
				'#value' => 'Start',
				'#submit' => array('eechat_server_start_main')
		);

	return $form;
}

function eechat_server() {
	/*drupal_add_js ( drupal_get_path ( 'module', 'eechat' ) . '/js/eeleaderboard.js' );*/
	$urlp = "/up" ;
	list ( $data, $request_headers ) = _doHTTP ( $urlp );
	$markup = "<div id=\"block-eechat-server\">\n";
	$markup = $markup . "<p class=\"helpbox server-description\">\n";
	$markup = $markup . "This page shows the current status of our servers.";
	$markup = $markup . "</p>\n";
	$markup = $markup . "<div class=\"fancy-separator\"></div>\n";
	
	$main_status = "down";
	$main_status_text = "Server not responding";
	
	if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$clansdata = json_decode ( $data );
		$main_status = "up";
		$main_status_text = "Server accepting requests";
	}
	else if (isset ( $request_headers->http_code ) && $request_headers->http_code != 0) { 
		$main_status_text = "Server up, but not accepting requests. Response code " . $request_headers->http_code ;
	}
		
	$markup = $markup . "<div class=\"server-status main-server\">\n";
	$markup = $markup . "<h2>Developer Server (TAWD)</h2>";
	$markup = $markup . "<label>Developer Server (TAWD)</label>";
	$markup = $markup . "<span class=\"status\">" . $main_status . "</span>";
	$markup = $markup . "<span class=\"status-detail\">" . $main_status_text . "</span>";
	$markup = $markup . "</div>\n";
	
	
	$markup = $markup . "</div>\n";
	
	$page = array (
			'#type' => 'markup',
			'#markup' => $markup 
	);
	return $page;
}


