<?php

function eechat_chat_submit_callback($form, $form_state) {
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
	return $element;
}

function eechat_chat_form($form, &$form_state) {
	/* Get characters */
	global $user;
	$urlp = "/user/" . urlencode ( $user->name );
	list ( $data, $request_headers ) = _doHTTP ( $urlp );
	
	if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$userdata = json_decode ( $data );
		$options = array ();
		if ($userdata->characters != null) {
			// Populate channel list
			$channels = array ();
			if (in_array ( "administrator", $user->roles )) {
				$channels ["*SysChat"] = "SysChat";
			}
			if (in_array ( "sages", $user->roles ) || in_array ( "administrator", $user->roles )) {
				$channels ["gm/earthsages"] = "Sage";
			}
			$channels ['rc/'] = 'Region';
			$channels ['tc/'] = 'Trade';
			
			// TODO won't work till server side chat buffer holds clan messages too
			// $channels['clan'] = 'Clan';
			
			// Populate character list
			foreach ( $userdata->characters as $character ) {
				$options [$character->name] = $character->name;
			}
			asort ( $options );
			
			$form ['character'] = array (
					'#type' => 'select',
					'#title' => t ( 'Character' ),
					'#options' => $options,
					'#description' => t ( 'The character which to send the chat message as.' ) 
			);
			
			$form ['message'] = array (
					'#title' => t ( 'says' ),
					'#type' => 'textfield',
					'#required' => TRUE,
					'#maxlength' => 252 
			);
			
			$form ['channel'] = array (
					'#type' => 'select',
					'#title' => t ( 'In' ),
					'#options' => $channels,
					'#description' => t ( 'The channel on which to send the chat message.' ),
					'#default_value' => 'rc/' 
			);
			
			$form ['box'] = array (
					'#type' => 'markup',
					'#prefix' => '<div id="box">',
					'#suffix' => '</div>',
					'#markup' => '' 
			);
			
			$form ['submit_button'] = array (
					'#type' => 'submit',
					'#value' => 'Send',
					'#ajax' => array (
							'callback' => 'eechat_chat_submit_callback',
							'wrapper' => 'box',
							'event' => 'click' 
					) 
			);
		} else {
			form_set_error ( 'message', t ( 'You must create a character in-game to be able to chat from the website.' ) );
		}
	}
	
	return $form;
}

function _render_chat_block($which_block) {
	if(user_access('view chat')) {
		list ( $data, $request_headers ) = _doHTTP ( "/chat" );
		if (isset ( $request_headers->http_code ) && $request_headers->http_code == 0) {
			$result = array (
					'#markup' => t ( 'Earth Eternal live chat is OFFLINE' )
			);
		} else if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
			$chat = json_decode ( $data );
			$markup = "<div id=\"block-eechat-" . $which_block . "-chat\" class=\"chat-area\">\n<ul>\n";
			if ($chat != null) {
				$markup = $markup . _render_chat ( $chat );
			}
			$markup = $markup . "</ul>\n";
			$markup = $markup . "</div>\n";
			if(user_access('chat')) {
				if ($which_block == 'eechat') {
					$markup = $markup . "<div class=\"full-chat-info\">\n";
					$markup = $markup . "<a href=\"/chat\">Chat Now!</a>\n";
					$markup = $markup . "</div>\n";
				}
			}
			$markup = $markup . "<script type=\"text/javascript\">\n";
			$markup = $markup . "var objDiv = document.getElementById(\"block-eechat-" . $which_block . "-chat\");";
			$markup = $markup . "objDiv.scrollTop = document.chatScrollPos ? document.chatScrollPos : objDiv.scrollHeight;\n";
			$markup = $markup . "objDiv.addEventListener(\"scroll\", function() { if (objDiv.scrollHeight - objDiv.scrollTop === objDiv.clientHeight) { document.chatScrollPos = false; } else { document.chatScrollPos = objDiv.scrollTop; } })\n";
			$markup = $markup . "</script>\n";
			$result = array (
					'#markup' => $markup
			);
		} else {
			$result = array (
					'#markup' => t ( 'Earth Eternal live chat not available. Status %status', array (
							'%status' => $request_headers->http_code
					) )
			);
		}
		return $result;
	}
	else {
		return "";
	}
}


function _render_chat($chat) {
	$markup = "";
	foreach ( $chat as $message ) {
		
		$doing = $m = 0 === strpos ( $message->message, "*" );
		$msg = $message->message;
		
		if ($doing) {
			$msg = substr ( $msg, 1 );
			while ( strpos ( $msg, "*", strlen ( $msg ) - strlen ( "*" ) ) !== false )
				$msg = substr ( $msg, 0, strlen ( $msg ) - 1 );
		}
		
		if ($message->channel->friendly != '/syschat' && $message->channel->friendly != 'NULL') {
			$m = $message->sender;
			if (! $doing)
				$m = $m . " says: " . $msg;
			else
				$m = $m . " " . strtolower ( substr ( $msg, 0, 1 ) ) . substr ( $msg, 1 );
		} else
			$m = $msg;
		
		$clazz = "channel-" . str_replace ( "/", "", $message->channel->friendly );
		
		if ($doing)
			$clazz = $clazz . " doing";
		
		//$m = check_markup(htmlentities($m), "filtered_html", "", false);
		$m = check_markup(htmlentities($m), "filtered_html", "", true);
		$m = substr($m,3, strlen($m) - 7);
		
		$markup = $markup . "<li class=\"" . $clazz . "\"><span class=\"chat-timestamp\">" . htmlentities ( $message->timeReadable ) . "</span> - <span class=\"chat-message\">" . $m . "</span></li>";
	}
	return $markup;
}
