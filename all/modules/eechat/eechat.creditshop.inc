<?php


function eechat_buy_form($form, &$form_state) {
	$parts = explode ( "/", request_path () );
	$itemname = $parts [sizeof ( $parts ) - 1];
	
	/* Get item */
	$urlp = "/cs/" . urlencode ( $itemname );
	list ( $data, $request_headers ) = _doHTTP ( $urlp );
	if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$itemdata = json_decode ( $data );
	} else {
		form_set_error ( 'message', t ( 'Unable to retrieve credit shop data.' ) );
		return $form;
	}
	
	/* Get characters */
	global $user;
	$urlp = "/user/" . urlencode ( $user->name );
	list ( $data, $request_headers ) = _doHTTP ( $urlp );
	
	if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$userdata = json_decode ( $data );
	} else {
		form_set_error ( 'message', t ( 'Unable to retrieve credit shop data.' ) );
		return $form;
	}
	
	if ($userdata->characters === null) {
		form_set_error ( 'message', t ( 'You must create a character in-game to be able to buy items from the website.' ) );
		return $form;
	}
	
	$options = array ();
	// Populate character list
	foreach ( $userdata->characters as $character ) {
		$options [$character->name] = $character->name;
	}
	asort ( $options );
	$form ['shopitem'] = array (
			'#type' => 'markup',
			'#markup' => _render_cs_item ( $itemdata ) 
	);
	
	$form ['character'] = array (
			'#type' => 'select',
			'#title' => t ( 'Character' ),
			'#options' => $options,
			'#description' => t ( 'The character which to send the item to.' ) 
	);
	
	$form ['qty'] = array (
			'#title' => t ( 'Quantity' ),
			'#type' => 'textfield',
			'#required' => TRUE,
			'#maxlength' => 4,
			'#default_value' => "1" 
	);
	
	$form ['item'] = array (
			'#title' => t ( 'Quantity' ),
			'#type' => 'hidden',
			'#required' => TRUE,
			'#default_value' => $itemname 
	);
	
	/*
	 * $form['box'] = array(
	 * '#type' => 'markup',
	 * '#prefix' => '<div id="box">',
	 * '#suffix' => '</div>',
	 * '#markup' => '',
	 * );
	 *
	 * $form['submit_button'] = array(
	 * '#type' => 'submit',
	 * '#value' => 'Send',
	 * '#ajax' => array(
	 * 'callback' => 'eechat_chat_submit_callback',
	 * 'wrapper' => 'box',
	 * 'event' => 'click'
	 * ),
	 * );
	 */
	$form ['submit_button'] = array (
			'#type' => 'submit',
			'#value' => 'Buy!' 
	);
	
	return $form;
}

function eechat_buy_form_validate($form, &$form_state) {
}
function eechat_buy_form_submit($form, &$form_state) {
	$data = array (
			'qty' => urlencode ( $form_state ['values'] ['qty'] ),
			'item' => urlencode ( $form_state ['values'] ['item'] ),
			'character' => urlencode ( $form_state ['values'] ['character'] ) 
	);
	list ( $data, $request_headers ) = _doHTTP ( "/cs", "POST", $data );
	if (isset ( $request_headers->http_code ) && $request_headers->http_code == 200) {
		drupal_set_message ( "Item purchased.", $type = 'status' );
	} else {
		form_set_error ( 'message', t ( 'Failed to buy item. ' ) . " " . $data );
	}
}


function eechat_creditshop($category = 'consumables', $start = 0) {
	drupal_add_js ( drupal_get_path ( 'module', 'eechat' ) . '/js/eeleaderboard.js' );
	$urlp = "/cs?category=" . strtoupper ( $category ) . "&count=100&start=" . $start;
	list ( $data, $request_headers ) = _doHTTP ( $urlp );
	$markup = "<div id=\"block-eechat-creditshop\">\n";
	if (isset ( $request_headers->http_code ) && $request_headers->http_code == 0) {
		$markup = $markup . t ( 'Earth Eternal is OFFLINE' );
	} else if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$creditshopdata = json_decode ( $data );
		
		$markup = $markup . "<p class=\"credit-shop-help helpbox\">\n";
		$markup = $markup . "The credit shop is our special store for items that are not restricted by your progress through \n";
		$markup = $markup . "the game, i.e. you can usually buy items at any level. It consists of both cosmetic and \n";
		$markup = $markup . "gameplay affecting items. Some items required credits, some require gold, and some require both! \n";
		$markup = $markup . "You can also access the credit shop in game by clicking on the <i>Credit Shop</i> in your toolbar.\n";
		$markup = $markup . "</p class=\"credit-shop-help helpbox\">\n";
		
		if ($creditshopdata->data != null) {
			
			$markup = $markup . "<div class=\"credit-shop-category\">\n";
			foreach ( $creditshopdata->data as $shopitem ) {
				$markup = $markup . _render_cs_item ( $shopitem, true );
			}
			$markup = $markup . "</div>\n";
		} else {
			$markup = $markup . "<div class=\"creditshop-not-ready\">\n";
			$markup = $markup . "The credit shop is not ready, please wait.";
			$markup = $markup . "</div>\n";
		}
	} else {
		if ($request_headers->http_code == 404) {
			$markup = $markup . "<div class=\"no-leaderboard\">";
			$markup = $markup . "This credit shop does not exist.";
			$markup = $markup . "</div>\n";
		} else {
			$markup = t ( 'Earth Eternal credit shop not available. ' . $account->name . '. Status %status', array (
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


function _render_cs_item($shopitem, $showbuy = false) {
	$markup = "<div class=\"credit-shop-item\">\n";
	$markup = $markup . "<h1>" . $shopitem->computedTitle . "</h1>\n";
	$markup = $markup . "<div class=\"icon\">\n";
	$markup = $markup . "<div class=\"icon-background\" style=\"background-image: url(/sites/default/files/gameicons/" . $shopitem->icon2 . "); \">\n";
	$markup = $markup . "<img class=\"icon-foreground\" src=\"/sites/default/files/gameicons/" . $shopitem->icon1 . "\"/>\n";
	$markup = $markup . "</div>\n";
	$markup = $markup . "</div>\n";
	$markup = $markup . "<div class=\"status\">\n";
	$markup = $markup . "<div class=\"status-icon status-icon-" . $shopitem->status . "\">\n";
	$markup = $markup . "</div>\n";
	$markup = $markup . "</div>\n";
	$markup = $markup . "<div class=\"description\">" . $shopitem->description . "</div>\n";
	$markup = $markup . "<hr class=\"clearfix fancy-small-divider\"/>\n";
	$markup = $markup . "<div class=\"prices\">\n";
	if ($shopitem->currency === 'CREDITS' || $shopitem->currency === 'COPPER+CREDITS') {
		$markup = $markup . "<div class=\"clearfix price credit-price\">\n";
		$markup = $markup . "<label>Credits</label>\n";
		$markup = $markup . "<span>" . $shopitem->credits . "</span>\n";
		$markup = $markup . "</div>\n";
	}
	if ($shopitem->currency === 'COPPER' || $shopitem->currency === 'COPPER+CREDITS') {
		$markup = $markup . "<div class=\"clearfix price copper-price\">\n";
		$markup = $markup . "<label>Copper</label>\n";
		$markup = $markup . "<span>" . _render_currency($shopitem->copper) . "</span>\n";
		$markup = $markup . "</div>\n";
	}
	$markup = $markup . "</div>\n";
	$markup = $markup . "<div class=\"clearfix\"></div>\n";
	if ($showbuy) {
		$markup = $markup . "<div class=\"item-actions\">\n";
		$markup = $markup . "<a class=\"small-styled-button item-action item-action-buy\" href=\"/buy/" . $shopitem->id . "\">Buy!</a>\n";
		$markup = $markup . "</div>\n";
	}
	
	$markup = $markup . "</div>\n";
	
	return $markup;
}

/**
* @} End of "defgroup eechat".
*/
