<?php

require_once dirname(__FILE__) . '/eechat.common.inc';

function eechat_character($id = '0') {
	$urlp = "/character/" . urlencode ( $id );
	list ( $data, $request_headers ) = _doHTTP ( $urlp );
	$markup = "<div id=\"block-eechat-character\">\n";
	if (isset ( $request_headers->http_code ) && $request_headers->http_code == 0) {
		$markup = $markup . t ( 'Earth Eternal is OFFLINE' );
	} else if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$characterdata = json_decode ( $data );
		
		$markup = $markup . "<h2 class=\"character-name\">" . $characterdata->cdef->css->display_name . "</h2>\n";
		$markup = $markup . "<h3 class=\"level\">Level " . $characterdata->cdef->css->level . "</h3>\n";
		$markup = $markup . "<h4 class=\"status\">" . $characterdata->status . "</h4>\n";
		if (isset ( $characterdata->clanName )) {
			$markup = $markup . "<h5 class=\"clan\"><a href=\"/clan/" . $characterdata->clanName . "\">A member of Clan " . $characterdata->clanName . "</a></h4>\n";
		}
		
		$markup = $markup . "<div class=\"fancy-separator\"></div>\n";
		
		// Date Created
		$markup = $markup . "<div class=\"created\">\n";
		$markup = $markup . "<span class=\"created-label character-profile-label\">Date Created</span>\n";
		$markup = $markup . "<span class=\"created-value character-profile-value\">" . date ( "Y-m-d, h:i A", $characterdata->createdTime ) . "</span>\n";
		$markup = $markup . "</div>\n";
		
		// Date Last Logged In
		$markup = $markup . "<div class=\"last-login\">\n";
		$markup = $markup . "<span class=\"last-login-label character-profile-label\">Last Seen</span>\n";
		$markup = $markup . "<span class=\"last-login-value character-profile-value\">" . $characterdata->lastLogon . "</span>\n";
		$markup = $markup . "</div>\n";
		
		// Race
		$markup = $markup . "<div class=\"race\">\n";
		$markup = $markup . "<span class=\"race-label character-profile-label\">Race</span>\n";
		$markup = $markup . "<span class=\"race-value character-profile-value\"><a href=\"/" . strtolower ( _get_race_name ( $characterdata->appearance->r ) ) . "\">" . _get_race_name ( $characterdata->appearance->r ) . "</a></span>\n";
		$markup = $markup . "</div>\n";
		
		// Class
		$markup = $markup . "<div class=\"charclass\">\n";
		$markup = $markup . "<span class=\"charclass-label character-profile-label\">Class</span>\n";
		
		$markup = $markup . "<span class=\"charclass-value character-profile-value\">" . _get_class_name ( $characterdata->cdef->css->profession ) . "</span>\n";
		$markup = $markup . "</div>\n";
		
		$markup = $markup . "<div class=\"fancy-separator\"></div>\n";
		
		// Stats
		$markup = $markup . "<div class=\"player-stats\">\n";
		$markup = $markup . _get_stats_html ( $characterdata->playerStats );
		$markup = $markup . "</div>\n";
	} else {
		if ($request_headers->http_code == 404) {
			$markup = $markup . "<div class=\"no-character\">";
			$markup = $markup . "This character does not exist.";
			$markup = $markup . "</div>\n";
		} else {
			$markup = t ( 'Earth Eternal character not available. ' . $id . '. Status %status', array (
					'%status' => $request_headers->http_code 
			) );
		}
	}
	$markup = $markup . "</div>\n";
	
	$page = array (
			'#type' => 'markup',
			'#markup' => $markup 
	);
	return $page;
}


function eechat_leaderboard($board = 'character', $sort = 'kills', $start = 0) {
	drupal_add_js ( drupal_get_path ( 'module', 'eechat' ) . '/js/eeleaderboard.js' );
	$urlp = "/leaderboard?board=" . $board . "&start=" . $start . "&sort=" . $sort;
	list ( $data, $request_headers ) = _doHTTP ( $urlp );
	$markup = "<div id=\"block-eechat-leaderboard\">\n";
	if (isset ( $request_headers->http_code ) && $request_headers->http_code == 0) {
		$markup = $markup . t ( 'Earth Eternal is OFFLINE' );
	} else if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$leaderdata = json_decode ( $data );
		
		if ($leaderdata->data != null) {
			
			$markup = $markup . "<table class=\"leader-table\">\n";
			foreach ( $leaderdata->data as $leader ) {
				$markup = $markup . "<tr class=\"leader-row\">\n";
				$markup = $markup . "<td class=\"leader-rank\">" . $leader->rank . "</td>\n";
				$markup = $markup . "<td><a href=\"/character/" . urlencode ( $leader->name ) . "\">" . $leader->name . "</a></td>\n";
				switch ($sort) {
					case 'kills' :
						$markup = $markup . "<td>" . $leader->stats->kills . "</td>\n";
						break;
					case 'deaths' :
						$markup = $markup . "<td>" . $leader->stats->deaths . "</td>\n";
						break;
					case 'pvpKills' :
						$markup = $markup . "<td>" . $leader->stats->pvpKills . "</td>\n";
						break;
					case 'pvpDeaths' :
						$markup = $markup . "<td>" . $leader->stats->pvpDeaths . "</td>\n";
						break;
				}
				$markup = $markup . "</tr>\n";
			}
			$markup = $markup . "</table>\n";
			$markup = $markup . "<div class=\"fancy-separator\"></div>\n";
			$markup = $markup . "<div class=\"leader-actions\">\n";
			$size = sizeof ( $leaderdata->data );
			if ($start + 10 < $size)
				$markup = $markup . "<a href=\"/leaderboard/" . $board . "/" . $sort . "/" . ($start + 10) . "\" class=\"leader-actions\">Next</a>\n";
			if ($start > 0)
				$markup = $markup . "<a href=\"/leaderboard/" . $board . "/" . $sort . "/" . max ( 0, $start - 10 ) . "\" class=\"leader-actions\">Previous</a>\n";
			$markup = $markup . "</div>\n";
		} else {
			$markup = $markup . "<div class=\"leaderboard-not-ready\">\n";
			$markup = $markup . "The leaderboard is not ready, please wait.";
			$markup = $markup . "</div>\n";
		}
	} else {
		if ($request_headers->http_code == 404) {
			$markup = $markup . "<div class=\"no-leaderboard\">";
			$markup = $markup . "This leaderboard does not exist.";
			$markup = $markup . "</div>\n";
		} else {
			$markup = t ( 'Earth Eternal leaderboard not available. ' . $account->name . '. Status %status', array (
					'%status' => $request_headers->http_code 
			) );
		}
	}
	$markup = $markup . "</div>\n";
	
	$page = array (
			'#type' => 'markup',
			'#markup' => $markup 
	);
	return $page;
}

