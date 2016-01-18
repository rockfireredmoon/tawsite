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
	$params = $_POST;
	
	$setuser = "";
	$setscore = 0;
	
	if (array_key_exists ( 'verify', $params )) {
		// NOTE: FINALLY found the cause of the failing to decrypt! It turned out that
		// although the longer key of the two below is used in the ActionScript, this 
		// is actually truncated to 128 bits (i.e. 16 chararacters). So simply halving
		// the key is enough for it work.
		
		$key = variable_get ( 'eechat_key', t ( 'XXXXXXXXXXXXXXXX' ) );
		
		$ver = $params['verify'];
		$h2b = hex2bin($ver);
		
		$plaintext_dec = rtrim(mcrypt_decrypt(MCRYPT_RIJNDAEL_128, $key, $h2b, MCRYPT_MODE_ECB));
		
		$score_fields = explode("|",$plaintext_dec);
	
		$setuser = $score_fields[0];	
		$setscore = intval($score_fields[1]);
	}
	
	drupal_add_http_header ( 'Content-Type', 'text/xml' );
	print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";

	print "<data>\r\n";
	print "<game>\r\n";
	
	$players = array ();
	
	global $user;
	
	$allusers = entity_load('user');
	foreach ($allusers as $user_id => $gameuser) {
		$edit = (array) $gameuser;
		$score = "0";
		if(array_key_exists('field_shroomies_score', $edit) && array_key_exists('und', $edit['field_shroomies_score'])  ) {
			$score = $edit['field_shroomies_score']['und'][0]['value'] . "";
		}
		if(strlen($score) == 0) {
			$score = "0";
		}
			
		if(strlen($gameuser->name) > 0 && $setuser === $gameuser->name) {
			if(array_key_exists('field_shroomies_score', $edit) && array_key_exists('und', $edit['field_shroomies_score'])  ) {
				$edit['field_shroomies_score']['und'][0]['value'] = "" . $setscore . "";
				user_save($gameuser, $edit);
			}
			$score = "" . $setscore;
		}
		
		if(strlen($gameuser->name) > 0)
			$players[$gameuser->name] = $score;
	}
	arsort($players);
	$c = 1;
	foreach ($players as $username => $score) {
		if($c > 10)
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

function stringToArray($s) {
    $r = array();
    for($i=0; $i<strlen($s); $i++) 
         $r[$i] = $s[$i];
    return $r;
}

function pad($data, $blocksize = 16) {
	$pad = $blocksize - (strlen($data) % $blocksize);
	return $data . str_repeat(chr($pad), $pad);
}

function _user_to_player_record($user) {
	if(in_array('und',$user->field_shroomies_score) && count($user->field_shroomies_score['und']) > 0 && in_array($user->field_shroomies_score['und'][0],'value')) {
		$score = $user->field_shroomies_score['und'][0]['value'];
	}
	if(strlen($score) == 0) {
		$score = 0;
	}
	return array (
			'name' => $user->name,
			'score' => $score
	);
}

