<?php

require_once dirname(__FILE__) . '/eechat.common.inc';


function eechat_server_stop($form, &$form_state) {
	//form_set_error ( 'message', t ( 'You must create a character in-game to be able to chat from the website.' ) );

	$server_name = explode(" ", $form_state['values'][$form_state['triggering_element']['#name']])[1];
	$service_name=$form_state['values']["service_name_" . $server_name ];

	// Sanity check service name
	if($service_name !== "iceee" && $service_name !== "tawd" && $service_name !== "vald" && $service_name !== "eetestd") {
		throw new Exception("An incorrect service name was provided.");
	}

	$element = $form ['box_' . $server_name];
	$element ['#markup'] = "<h1>DONE!</h1>";
	$_SESSION['stopping_' . $server_name] = true;
	$_SESSION['starting_' . $server_name] = false;
	$_SESSION['state_expire_' . $server_name ] = time() + 30;

	_run_ssh("systemctl stop " . $service_name);

	return $element;
}

function eechat_server_start($form, &$form_state) {

	$server_name = explode(" ", $form_state['values'][$form_state['triggering_element']['#name']])[1];
	$service_name=$form_state['values']["service_name_" . $server_name ];

	// Sanity check service name
	if($service_name !== "iceee" && $service_name !== "tawd" && $service_name !== "vald" && $service_name !== "eetestd") {
		throw new Exception("An incorrect service name was provided.");
	}

	$element = $form ['box_' . $server_name ];
	$element ['#markup'] = "<h1>DONE!</h1>";
	$_SESSION['stopping_' . $server_name ] = false;
	$_SESSION['starting_' . $server_name ] = true;
	$_SESSION['state_expire_' . $server_name ] = time() + 30;

	_run_ssh("systemctl start " . $service_name );

	return $element;
}

function eechat_server_form($form, &$form_state) {
	global $user;

	// Info
	$markup = "<div id=\"server-status\">\n";
	$markup = $markup . "<p class=\"helpbox server-description\">\n";
	$markup = $markup . "This page shows the current status of our servers. NOTE. FOR NOW, YOU WILL HAVE TO MANUALLY REFRESH THIS PAGE. THIS IS TEMPORARY";
	$markup = $markup . "</p>\n";
	$markup = $markup . "<div class=\"fancy-separator\"></div>\n";
	$markup = $markup . "</div>\n";

	$form ['box_info'] = array (
			'#type' => 'markup',
			'#markup' => $markup
	);

	_build_server_status_markup("dev", "tawd", "Development (TAWD)", $form);
	//_build_server_status_markup("legacydev", "vald", "Legacy Development (VALD)", $form);
	_build_server_status_markup("main", "vald", "Official (VALD)", $form);
	_build_server_status_markup("legacy", "eetestd", "Testing (EETESTD)", $form);

	return $form;
}

function _build_server_status_markup($server_name, $service_name, $server_fullname, &$form) {
	// Make sure the current state transition has not expired
	if(!isset($_SESSION['state_expire_' . $server_name ]) || ( isset($_SESSION['state_expire_' . $server_name]) && time() > $_SESSION['state_expire_' . $server_name])) {
		$_SESSION['stopping_' . $server_name] = false;
		$_SESSION['starting_' . $server_name] = false;
	}

	// Dev server
	$main_status = "down";
	$main_status_text = "Server is not running";

	if(isset($_SESSION['starting_' . $server_name]) && $_SESSION['starting_' . $server_name]) {
		$main_status = "starting";
		$main_status_text = "You have requested the server start. Please wait";
	}
	else if(isset($_SESSION['stopping_' . $server_name]) && $_SESSION['stopping_' . $server_name]) {
		$main_status = "stopping";
		$main_status_text = "You have requested the server stop. Please wait";
	}
	else {
		if(strpos(_run_ssh("systemctl status " . $service_name), "Active: active") !== FALSE) {
			$main_status = "up";
			$main_status_text = "Server is running";
		}
	}

	$markup = "<div class=\"server-status " . $server_name . "-server\">\n";
	$markup = $markup . "<h2>" . $server_fullname . "</h2>";
	$markup = $markup . "<label class=\"status status-" . $main_status . "\">" . $main_status . "</label>";
	$markup = $markup . "<span class=\"status-detail\">" . $main_status_text . "</span>";
	$markup = $markup . "</div>\n";

	$form ['box_' . $server_name] = array (
			'#type' => 'markup',
			'#prefix' => '<div class="server-status-box server-status-box-' . $server_name . '">',
			'#suffix' => '</div>',
			'#markup' => $markup
	);

	$form ['service_name_' . $server_name ] = array (
			'#type' => 'hidden',
			'#value' => $service_name
	);

	if($main_status === "up")
		$form ['stop_' . $server_name] = array (
				'#type' => 'submit',
				'#value' => 'Stop ' . $server_name,
				'#submit' => array('eechat_server_stop')
	);
	else if($main_status === "down")
		$form ['start_' . $server_name ] = array (
				'#type' => 'submit',
				'#value' => 'Start ' . $server_name,
				'#submit' => array('eechat_server_start')
	);
}

function _run_ssh($command) {

	$connection = ssh2_connect('live.theanubianwar.com', 22);
	if(!$connection) {
		throw new Exception("Cannot connect to server");
	}
	ssh2_auth_pubkey_file($connection, 'root', '/var/keys/tawd-id_ecdsa.pub', '/var/keys/tawd-id_ecdsa');
	$stream = ssh2_exec($connection, $command);

	$errorStream = ssh2_fetch_stream($stream, SSH2_STREAM_STDERR);

	stream_set_blocking($errorStream, true);
	stream_set_blocking($stream, true);

	$output = stream_get_contents($stream);
	stream_get_contents($errorStream);

	fclose($errorStream);
	fclose($stream);

	return $output;
}


?>
