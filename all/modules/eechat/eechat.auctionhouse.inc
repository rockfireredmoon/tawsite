<?php

function eechat_auctionhouse($auctioneer = 'Shorn Kelly', $start = 0) {
	//drupal_add_js ( drupal_get_path ( 'module', 'eechat' ) . '/js/eeleaderboard.js' );
	$urlp = "/auction/" . urlencode ( $auctioneer );
	list ( $data, $request_headers ) = _doHTTP ( $urlp );
	$markup = "<div id=\"block-eechat-auctionhouse\">\n";
	if (isset ( $request_headers->http_code ) && $request_headers->http_code == 0) {
		$markup = $markup . t ( 'Earth Eternal is OFFLINE' );
	} else if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$auctionhousedata = json_decode ( $data );
		
		$markup = $markup . "<p class=\"auction-house-help helpbox\">\n";
		$markup = $markup . "The auction is the store for items that are sold by players. Some items require credits, some require gold, and some require both! \n";
		$markup = $markup . "You can also access the auction house in game by finding the auctioneer and clicking on him or her.\n";
		$markup = $markup . "</p>\n";
		
		$markup = $markup . "<div class=\"auction-house\">\n";
		foreach ( $auctionhousedata as $key => $value ) {
			$markup = $markup . _render_ah_item (  $value, true );
		}
		$markup = $markup . "</div>\n";
		
	} else {
		if ($request_headers->http_code == 404) {
			$markup = $markup . "<div class=\"no-auctionhouse\">";
			$markup = $markup . "This auction house does not exist.";
			$markup = $markup . "</div>\n";
		} else {
			$markup = t ( 'Earth Eternal auction house not available. ' . $account->name . '. Status %status', array (
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


function _render_ah_item($ahitem, $showbuy = false) {
	// Convert remaining time to readable string
	$remainseconds = $ahitem->end - time();
	if($remainseconds < 0) {
		$remaintime = "Expired!";
	}
	else if($remainseconds < 60) {
		$remaintime = gmdate("s", $remainseconds) . " seconds";
	}
	else if($remainseconds < 3600) {
		$remaintime = gmdate("i", $remainseconds) . " minutes " . gmdate("s", $remainseconds) . " seconds";
	}
	else if($remainseconds < 86400) {
		$remaintime = gmdate("H", $remainseconds) . " hours " . gmdate("i", $remainseconds) . " minutes";
	}
	else {
		$remaintime = gmdate("d", $remainseconds) . " days" . gmdate("H", $remainseconds) . " hours";
	}

	$remainclass = "normal";
	if($remainseconds < 60)
		$remainclass = "expired";
	else if($remainseconds < 600)
		$remainclass = "soon";
	
	$markup = "<div class=\"auction-house-item\">\n";
	//$markup = $markup . "<h1>" . $ahitem->item->displayName . "</h1>\n";
	
	$markup = $markup . "<div class=\"icon\">\n";
	$markup = $markup . "<div class=\"icon-inner\">\n";
	$markup = $markup . "<div class=\"icon-background\" style=\"background-image: url(/sites/default/files/gameicons/" . $ahitem->item->icon2 . "); \">\n";
	$markup = $markup . "<img class=\"icon-foreground\" src=\"/sites/default/files/gameicons/" . $ahitem->item->icon1 . "\"/>\n";
	$markup = $markup . "</div>\n";
	$markup = $markup . "</div>\n";
	$markup = $markup . "</div>\n";

	$markup = $markup . "<div class=\"auction-house-item-details\">\n";

	// Top row
	$markup = $markup . "<div class=\"auction-house-item-top auction-house-item-row\">\n";
	$markup = $markup . "<div class=\"item-name quality-" . $ahitem->item->qualityLevel . "\">\n";
	$markup = $markup . $ahitem->item->displayName . "\n";
	$markup = $markup . "</div>\n";
	
	$markup = $markup . "<div class=\"item-seller\">\n";
	$markup = $markup . $ahitem->sellerDetail->name . "\n";
	$markup = $markup . "</div>\n";
	$markup = $markup . "</div>\n";
	
	// Middle row
	$markup = $markup . "<div class=\"auction-house-item-middle auction-house-item-row\">\n";

	$markup = $markup . "<div class=\"item-level\">\n";
	$markup = $markup . "Level " . $ahitem->item->level . "\n";
	$markup = $markup . "</div>\n";
	$markup = $markup . "<div class=\"item-type\">\n";
	$markup = $markup . _get_item_type_name($ahitem->item->type) . "\n";
	if($ahitem->item->type === ItemType::WEAPON) {
		$markup = $markup . " " . _get_weapon_type_name($ahitem->item->weaponType) . "\n";
	}
	$markup = $markup . "</div>\n";
	$markup = $markup . "<div class=\"item-time " . $remainclass . "\">\n";
	$markup = $markup . $remaintime . "\n";
	
	$markup = $markup . "</div>\n";
	
	$markup = $markup . "</div>\n";
	
	// Bottom row
	$markup = $markup . "<div class=\"auction-house-item-bottom auction-house-item-row\">\n";

	if($ahitem->buyItNowCopper > 0 || $ahitem->buyItNowCredits > 0) { 
		$markup = $markup . "<span class=\"buy-label\">Buy:</span>\n";
	}
	if($ahitem->buyItNowCopper > 0) {
		$markup = $markup . "<div class=\"buy-it-now-copper\">\n";
		$markup = $markup . _render_currency($ahitem->buyItNowCopper);
		$markup = $markup . "</div>\n";
	}
	
	if($ahitem->buyItNowCredits > 0) {
		$markup = $markup . "<div class=\"buy-it-now-credits\">\n";
		$markup = $markup . "<div class=\"clearfix price credit-price\">\n";
		$markup = $markup . "<label>Credits</label>\n";
		$markup = $markup . "<span>" . $ahitem->buyItNowCopper . "</span>\n";
		$markup = $markup . "</div>\n";
		$markup = $markup . "</div>\n";
	}
	
	$bidcount = sizeof($ahitem->bids);
	if($bidcount > 0) {
		$highbid = $ahitem->bids[$bidcount -1]; 
		$markup = $markup . "<span class=\"hibid-label\">High:</span>\n";
		
		if($highbid->copper > 0) {
			$markup = $markup . "<div class=\"hibid-copper\">\n";
			$markup = $markup . _render_currency($highbid->copper);
			$markup = $markup . "</div>\n";
		}
		
		if($highbid->credits > 0) {
			$markup = $markup . "<div class=\"hibid-credits\">\n";
			$markup = $markup . "<div class=\"clearfix price credit-price\">\n";
			$markup = $markup . "<label>Credits</label>\n";
			$markup = $markup . "<span>" . $highbid->credits . "</span>\n";
			$markup = $markup . "</div>\n";
			$markup = $markup . "</div>\n";
		}

		$markup = $markup . "<div class=\"bidcount\">\n";
		$markup = $markup . $bidcount . " bid" . ($bidcount > 1 ? "s" : "");
		$markup = $markup . "</div>\n";
	}
	else {
		$markup = $markup . "<div class=\"bidcount\">\n";
		$markup = $markup . "No bids";
		$markup = $markup . "</div>\n";
	}
	
	$markup = $markup . "</div>\n";

	$markup = $markup . "</div>\n";
	
	$markup = $markup . "</div>\n";
	return $markup;
}

/**
* @} End of "defgroup eechat".
*/
