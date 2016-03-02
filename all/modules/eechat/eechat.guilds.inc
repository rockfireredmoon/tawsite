<?php


function eechat_guild($id) {
	$urlp = "/guild/" . urlencode ( $id );
	list ( $data, $request_headers ) = _doHTTP ( $urlp );
	$markup = "<div id=\"block-eechat-guild\">\n";
	if (isset ( $request_headers->http_code ) && $request_headers->http_code == 0) {
		$markup = $markup . t ( 'Earth Eternal is OFFLINE' );
	} else if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$guilddata = json_decode ( $data );
		
		$markup = $markup . "<h2 class=\"guild-name\">" . $guilddata->name . "</h2>\n";
		$markup = $markup . "<h4 class=\"motto\">" . $guilddata->motto . "</h4>\n";
		// $markup = $markup . "<h4 class=\"status\">" . $characterdata->status . "</h4>\n";
		
		$markup = $markup . "<div class=\"fancy-separator\"></div>\n";
		
		// Stats
		// $markup = $markup . "<div class=\"player-stats\">\n";
		// $markup = $markup . _get_stats_html ( $clandata->playerStats );
		// $markup = $markup . "</div>\n";
		// $markup = $markup . "<div class=\"fancy-separator\"></div>\n";
		
		// Roster
		// $markup = $markup . "<h2 class=\"members\">Members</h3>\n";
		// $markup = $markup . "<table class=\"member-table\">\n";
		// foreach ( $clandata->members as $member ) {
		// $markup = $markup . "<tr class=\"member-row\">\n";
		//
		// $markup = $markup . "<td class=\"rank rank-" . strtolower(_get_rank($member->rank)) . "\">" . _get_rank($member->rank) . "</td>\n";
		// $markup = $markup . "<td class=\"member-name\"><a href=\"/character/" . urlencode ($member->name) . "\">" . $member->name . "</a></td>\n";
		// $markup = $markup . "<td class=\"member-level\">" . $member->level . "</td>\n";
		// $markup = $markup . "</tr>\n";
		// }
		// $markup = $markup . "</table>\n";
	} else {
		if ($request_headers->http_code == 404) {
			$markup = $markup . "<div class=\"no-guild\">";
			$markup = $markup . "This guild does not exist.";
			$markup = $markup . "</div>\n";
		} else {
			$markup = t ( 'Earth Eternal guild not available. ' . $id . '. Status %status', array (
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

function eechat_guilds($start = 0) {
	drupal_add_js ( drupal_get_path ( 'module', 'eechat' ) . '/js/eeleaderboard.js' );
	$urlp = "/guilds?start=" . $start;
	list ( $data, $request_headers ) = _doHTTP ( $urlp );
	$markup = "<div id=\"block-eechat-guilds\">\n";
	$markup = $markup . "<p class=\"helpbox guilds-description\">\n";
	$markup = $markup . "Guilds are one of the two social groupings available to players. Where <a href=\"/clans\">Clans</a> are informal ";
	$markup = $markup . "groupings of players, Guilds are different in that guilds are pre-formed and being a member of one will affect the quest ";
	$markup = $markup . "that you are able to perform and other gameplay affecting mechanics. You could think of your fellow <i>Clan</i> as your family, and your <i>Guild</i> as your colleagues!";
	$markup = $markup . "</p>\n";
	$markup = $markup . "<div class=\"fancy-separator\"></div>\n";
	if (isset ( $request_headers->http_code ) && $request_headers->http_code == 0) {
		$markup = $markup . t ( 'Earth Eternal is OFFLINE' );
	} else if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$guildsdata = json_decode ( $data );
		
		$markup = $markup . "<table class=\"guilds-table\">\n";
		$markup = $markup . "<thead>\n";
		$markup = $markup . "<tr>\n";
		$markup = $markup . "<td>Name</td>\n";
		$markup = $markup . "<td>Type</td>\n";
		$markup = $markup . "<td>Motto</td>\n";
		$markup = $markup . "</tr>\n";
		$markup = $markup . "</thead>\n";
		$markup = $markup . "<tbody>\n";
		foreach ( $guildsdata as $guild ) {
			$markup = $markup . "<tr class=\"guild-row\">\n";
			$markup = $markup . "<td class=\"guild-name\"><a href=\"/guild/" . urlencode ( $guild->name ) . "\">" . $guild->name . "</a></td>\n";
			$markup = $markup . "<td class=\"guild-type\">" . $guild->type . "</td>\n";
			$markup = $markup . "<td class=\"guild-motto\">" . $guild->motto . "</td>\n";
			$markup = $markup . "</tr>\n";
		}
		$markup = $markup . "</tbody>\n";
		$markup = $markup . "</table>\n";
		$markup = $markup . "<div class=\"fancy-separator\"></div>\n";
	} else {
		if ($request_headers->http_code == 404) {
			$markup = $markup . "<div class=\"no-clans\">";
			$markup = $markup . "No guilds exist.";
			$markup = $markup . "</div>\n";
		} else {
			$markup = t ( 'Earth Eternal guilds not available. ' . $account->name . '. Status %status', array (
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

