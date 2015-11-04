<?php

require_once dirname(__FILE__) . '/eechat.common.inc';

function eechat_clan($id) {
	$urlp = "/clan/" . urlencode ( $id );
	list ( $data, $request_headers ) = _doHTTP ( $urlp );
	$markup = "<div id=\"block-eechat-clan\">\n";
	if (isset ( $request_headers->http_code ) && $request_headers->http_code == 0) {
		$markup = $markup . t ( 'Earth Eternal is OFFLINE' );
	} else if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$clandata = json_decode ( $data );
		
		$markup = $markup . "<h2 class=\"clan-name\">" . $clandata->name . "</h2>\n";
		$markup = $markup . "<h4 class=\"motd\">" . $clandata->motd . "</h4>\n";
		// $markup = $markup . "<h4 class=\"status\">" . $characterdata->status . "</h4>\n";
		
		$markup = $markup . "<div class=\"fancy-separator\"></div>\n";
		
		// Date Created
		$markup = $markup . "<div class=\"created\">\n";
		$markup = $markup . "<span class=\"created-label clan-label\">Date Created</span>\n";
		$markup = $markup . "<span class=\"created-value clan-value\">" . date ( "Y-m-d, h:i A", $clandata->createdTime ) . "</span>\n";
		$markup = $markup . "</div>\n";
		
		$markup = $markup . "<div class=\"fancy-separator\"></div>\n";
		
		// Stats
		$markup = $markup . "<div class=\"player-stats\">\n";
		$markup = $markup . _get_stats_html ( $clandata->playerStats );
		$markup = $markup . "</div>\n";
		$markup = $markup . "<div class=\"fancy-separator\"></div>\n";
		
		// Roster
		$markup = $markup . "<h2 class=\"members\">Members</h3>\n";
		$markup = $markup . "<table class=\"member-table\">\n";
		foreach ( $clandata->members as $member ) {
			$markup = $markup . "<tr class=\"member-row\">\n";
			
			$markup = $markup . "<td class=\"rank rank-" . strtolower ( _get_rank ( $member->rank ) ) . "\">" . _get_rank ( $member->rank ) . "</td>\n";
			$markup = $markup . "<td class=\"member-name\"><a href=\"/character/" . urlencode ( $member->name ) . "\">" . $member->name . "</a></td>\n";
			$markup = $markup . "<td class=\"member-level\">" . $member->level . "</td>\n";
			$markup = $markup . "</tr>\n";
		}
		$markup = $markup . "</table>\n";
	} else {
		if ($request_headers->http_code == 404) {
			$markup = $markup . "<div class=\"no-clan\">";
			$markup = $markup . "This clan does not exist.";
			$markup = $markup . "</div>\n";
		} else {
			$markup = t ( 'Earth Eternal clan not available. ' . $id . '. Status %status', array (
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


function eechat_clans($start = 0) {
	drupal_add_js ( drupal_get_path ( 'module', 'eechat' ) . '/js/eeleaderboard.js' );
	$urlp = "/clans?start=" . $start;
	list ( $data, $request_headers ) = _doHTTP ( $urlp );
	$markup = "<div id=\"block-eechat-clans\">\n";
	$markup = $markup . "<p class=\"helpbox clans-description\">\n";
	$markup = $markup . "Clans are one of the two social groupings available to players. Where <a href=\"guilds\">Guilds</a> are fixed ";
	$markup = $markup . "groupings of players, membership of which will affect gameplay, Clans are different in that anybody can form one and ";
	$markup = $markup . "gameplay is not affected. You could think of your fellow <i>Clan</i> as your family, and your <i>Guild</i> as your colleagues!";
	$markup = $markup . "</p>\n";
	$markup = $markup . "<div class=\"fancy-separator\"></div>\n";
	if (isset ( $request_headers->http_code ) && $request_headers->http_code == 0) {
		$markup = $markup . t ( 'Earth Eternal is OFFLINE' );
	} else if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$clansdata = json_decode ( $data );
		
		$markup = $markup . "<table class=\"clans-table\">\n";
		$markup = $markup . "<thead>\n";
		$markup = $markup . "<tr>\n";
		$markup = $markup . "<td>Name</td>\n";
		$markup = $markup . "<td>Leader</td>\n";
		$markup = $markup . "<td>Members</td>\n";
		$markup = $markup . "</tr>\n";
		$markup = $markup . "</thead>\n";
		$markup = $markup . "<tbody>\n";
		foreach ( $clansdata as $clan ) {
			$markup = $markup . "<tr class=\"clans-row\">\n";
			$markup = $markup . "<td class=\"clan-name\"><a href=\"/clan/" . urlencode ( $clan->name ) . "\">" . $clan->name . "</a></td>\n";
			$markup = $markup . "<td class=\"clan-leader\"><a href=\"/character/" . urlencode ( $clan->leader->name ) . "\">" . $clan->leader->name . "</a></td>\n";
			$markup = $markup . "<td class=\"clan-size\">" . $clan->size . "</td>\n";
			$markup = $markup . "</tr>\n";
		}
		$markup = $markup . "</tbody>\n";
		$markup = $markup . "</table>\n";
		$markup = $markup . "<div class=\"fancy-separator\"></div>\n";
		$markup = $markup . "<div class=\"clans-actions\">\n";
		$size = sizeof ( $clansdata );
		if ($start + 10 < $size)
			$markup = $markup . "<a href=\"/leaderboard/" . $board . "/" . $sort . "/" . ($start + 10) . "\" class=\"leader-actions\">Next</a>\n";
		if ($start > 0)
			$markup = $markup . "<a href=\"/leaderboard/" . $board . "/" . $sort . "/" . max ( 0, $start - 10 ) . "\" class=\"leader-actions\">Previous</a>\n";
		$markup = $markup . "</div>\n";
	} else {
		if ($request_headers->http_code == 404) {
			$markup = $markup . "<div class=\"no-clans\">";
			$markup = $markup . "No clans exist.";
			$markup = $markup . "</div>\n";
		} else {
			$markup = t ( 'Earth Eternal clans not available. ' . $account->name . '. Status %status', array (
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


