<?php


function eechat_shroomies_search() {
	global $user;
	
	// (function ($) {
	// jQuery(document).ready(function() {
	// var al = $(".menu a.active");
	// al.closest("li.menu__item").addClass("is-active-trail");
	// });
	
	// })(jQuery);
	
	$markup = "<script type=\"text/javascript\" src=\"ShroomiesSearch/swfobject.js\"></script>\n";
	$markup = $markup . "<div id=\"flash-game\">\n";
	$markup = $markup . "    <script type=\"text/javascript\">\n";
	$markup = $markup . "    function embedFlashHead() {\n";
	$markup = $markup . "        var so = new SWFObject(\"ShroomiesSearch/ShroomieSearch.swf\", \"ShroomieSearch\", \"640\", \"480\", \"9\", \"000000\");\n";
	$markup = $markup . "        so.addParam(\"wmode\", \"transparent\");\n";
	$markup = $markup . "        so.addVariable(\"username\", \"" . $user->name . "\");\n";
	$markup = $markup . "        so.write(\"flash-game\");\n";
	$markup = $markup . "    }\n";
	$markup = $markup . "    (function ($) {\n";
	$markup = $markup . "        jQuery(document).ready(function() {\n";
	$markup = $markup . "            embedFlashHead();\n";
	$markup = $markup . "        });\n";
	$markup = $markup . "    })(jQuery);\n";
	$markup = $markup . "    </script>\n";
	$markup = $markup . "</div>\n";
	$page = array (
			'#type' => 'markup',
			'#markup' => $markup 
	);
	return $page;
}

function eechat_minigame_scores() {
	// gamename=shroomies%5Fsearch&verify=73d09a3d834790a96feb5f6eb322b437
	// http://stackoverflow.com/questions/2285944/returning-untemplated-output-in-drupal-from-menu-callback-function
	// https://www.drupal.org/node/1423126
	// http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/flash/net/URLLoaderDataFormat.html#includeExamplesSummary
	$params = $_POST;
	error_log ( "In post\n", 3, "/tmp/phpxx.log" );
	foreach ( $params as $key => $value ) {
		error_log ( ">> " . $key . " = " . $value . "\n", 3, "/tmp/phpxx.log" );
	}
	if (array_key_exists ( 'verify', $params )) {
// 		$ciphertext_base64 = $params->verify;
// 		$iv_size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_128, MCRYPT_MODE_ECB);
		
		//TODO protect this in configuration fwiw
		$key = "45$97$56842568#52157%526@872#3$5";
		$h2b = hex2bin($params['verify']);
// 		$h2b = $params['verify'];
		error_log ( "h2b " . $h2b . " (len: " . strlen($h2b) . "\n", 3, "/tmp/phpxx.log" );
		
// 		$ciphertext_dec = base64_decode ( $ciphertext_base64 );
// 		$ciphertext_dec = $ciphertext_base64;
// 		$ciphertext_dec = hex2bin ( $ciphertext_base64 );
		
		// retrieves the IV, iv_size should be created using mcrypt_get_iv_size()
// 		$iv_dec = substr ( $ciphertext_dec, 0, $iv_size );
		
		// retrieves the cipher text (everything except the $iv_size in the front)
// 		$ciphertext_dec = substr ( $ciphertext_dec, $iv_size );
		
		// may remove 00h valued characters from end of plain text
// 		$plaintext_dec = mcrypt_decrypt ( MCRYPT_RIJNDAEL_128, $key, $ciphertext_dec, MCRYPT_MODE_ECB, $iv_dec );

		$blockSize = mcrypt_get_block_size(MCRYPT_RIJNDAEL_128, MCRYPT_MODE_ECB);
		$padding = $blockSize - (strlen($h2b) % $blockSize);
		$h2b .= str_repeat(chr($padding), $padding);
		
		error_log ( "h2b2 " . $h2b . " (len: " . strlen($h2b) . "\n", 3, "/tmp/phpxx.log" );
		

		$iv = mcrypt_create_iv(mcrypt_get_iv_size(MCRYPT_RIJNDAEL_128, MCRYPT_MODE_ECB), MCRYPT_RAND);
		$plaintext_dec = rtrim(mcrypt_decrypt(MCRYPT_RIJNDAEL_128, $key, $h2b, MCRYPT_MODE_ECB, "\0"));
		
// 		$plaintext_dec = rtrim(
// 				mcrypt_decrypt(
// 						MCRYPT_RIJNDAEL_128,
// 						$key,
// 						$h2b,
// 						MCRYPT_MODE_ECB,
// 						null
// 				), "\0"
// 		);
		
		
		
		error_log ( "RESULT \n" . $plaintext_dec, 3, "/tmp/phpxx.log" );
	}
	// else {
	drupal_add_http_header ( 'Content-Type', 'text/xml' );
	print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";

	print "<data>\r\n";
	print "<game>\r\n";
	
	$players = array ();
	
	global $user;
	
	$allusers = entity_load('user');
	foreach ($allusers as $user_id => $gameuser) {
		try {
			$score = $gameuser->field_shroomies_score['und'][0]['value'];
		}
		catch (Exception $e) {
		}
		if(strlen($score) == 0) {
			$score = 0;
		}
		if(strlen($gameuser->name) > 0)
			$players[$gameuser->name] = $score;
	}
	arsort($players);
	$c = 1;
	foreach ($players as $username => $score) {
		if(c > 10)
			break;
		print "<player rank=\"" . $c . "\" name=\"" . $username . "\" score=\"" . $score . "\"/>\r\n";
		$c += 1;
	}
	
	print "</game>\r\n";
	print "<rank>\r\n";
	print "<player rank=\"" . (array_search($user->name,array_keys($players)) + 1) . "\" name=\"" . $user->name . "\" score=\"" . $players[$user->name] . "\"/>\r\n";
	print "</rank>\r\n";
	print "</data>\r\n";
	
	exit ( 0 );
}

function _user_to_player_record($user) {
	try {
		$score = $user->field_shroomies_score['und'][0]['value'];
	}
	catch (Exception $e) {
	}
	if(strlen($score) == 0) {
		$score = 0;
	}
	return array (
			'name' => $user->name,
			'score' => $score
	);
}

