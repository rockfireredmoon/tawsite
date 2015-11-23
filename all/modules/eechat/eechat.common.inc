<?php
class ItemType {
	const UNKNOWN = 0;
	const SYSTEM = 1;
	const WEAPON = 2;
	const ARMOR = 3;
	const CHARM = 4;
	const CONSUMABLE = 5;
	const CONTAINER = 6;
	const BASIC = 7;
	const SPECIAL = 8;
	const QUEST = 9;
	const RECIPE = 10;
}

class WeaponType {
	const NONE = 0;
	const SMALL = 1;
	const ONE_HAND = 2;
	const TWO_HAND = 3;
	const POLE = 4;
	const WAND = 5;
	const BOW = 6;
	const THROWN = 7;
	const ARCANE_TOTEM = 8;
}

class BindingType {
	const BIND_NEVER = 0;
	const BIND_ON_PICKUP = 1;
	const BIND_ON_EQUIP = 2;
}

$BINDING_TYPE_NAMES = array(
		BindingType::BIND_NEVER => "Never",
		BindingType::BIND_ON_PICKUP => "On Pickup",
		BindingType::BIND_ON_EQUIP => "On Equip",
);

function _render_currency($copper) {
	$gold = ( int ) ($copper / 10000);
	$silver = ( int ) (($copper - ($gold * 10000)) / 100);
	$copper = $copper - ($gold * 10000) - ($silver * 100);
	$m = "<div class=\"amount-value gold-value\"><span class=\"amount-icon gold-icon\">G</span><span class=\"amount gold-amount\">" . $gold . "</span><span class=\"suffix gold-suffix\">Gold</span></div>";
	$m .= "<div class=\"amount-value silver-value\"><span class=\"amount-icon silver-icon\">S</span><span class=\"amount silver-amount\">" . $silver . "</span><span class=\"suffix silver-suffix\">Silver</span></div>";
	$m .= "<div class=\"amount-value copper-value\"><span class=\"amount-icon copper-icon\">C</span><span class=\"amount copper-amount\">" . $copper . "</span><span class=\"suffix copper-suffix\">Copper</span></div>";
	return $m;
}
function _render_purse() {
	global $user;
	$urlp = "/user/" . urlencode ( $user->name ) . "?detailed=true";
	list ( $data, $request_headers ) = _doHTTP ( $urlp );
	$markup = "<div class=\"purse\">\n";
	if (isset ( $request_headers->http_code ) && $request_headers->http_code == 0) {
		$markup = $markup . t ( 'Earth Eternal is OFFLINE' );
	} else if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$userdata = json_decode ( $data );
		
		// Build character gold HTML separately (and get the totals along the way)
		$charmarkup = "<ul>\n";
		$totalcopper = 0;
		foreach ( $userdata->characters as $character ) {
			$charmarkup = $charmarkup . "<li>\n";
			$charmarkup = $charmarkup . "<span class=\"character-name\"><a href=\"/character/" . urlencode ( $character->name ) . "\">" . $character->name . "</a></span>\n";
			$copper = array_key_exists ( 'copper', $character->details->cdef->css ) ? $character->details->cdef->css->copper : 0;
			$charmarkup = $charmarkup . _render_currency ( $copper );
			$charmarkup = $charmarkup . "</li>";
			$totalcopper += 1;
		}
		$charmarkup = $charmarkup . "</ul>\n";
		
		$markup = $markup . "<div class=\"credits\">\n";
		$markup = $markup . "<span class=\"credits-value\">" . $userdata->credits . "</span>";
		$markup = $markup . "<label class=\"credits-label\">Credits</label>";
		$markup = $markup . "</div>\n";
		
		$markup = $markup . "<hr class=\"fancy-small-divider\"/>\n";
		$markup = $markup . "<div class=\"character-purse\">\n";
		$markup = $markup . $charmarkup;
		$markup = $markup . "</div>\n";
	}
	$markup = $markup . "</div>\n";
	return $markup;
}
function _get_stats_html($stats) {
	$markup = "<div class=\"total-kills\">\n";
	$markup = $markup . "<span class=\"total-kills-label player-stat-label\">Total Kills</span>\n";
	$markup = $markup . "<span class=\"total-kills-value player-stat-value\">" . $stats->kills . "</span>\n";
	$markup = $markup . "</div>\n";
	
	$markup = $markup . "<div class=\"total-deaths\">\n";
	$markup = $markup . "<span class=\"total-deaths-label player-stat-label\">Total Deaths</span>\n";
	$markup = $markup . "<span class=\"total-deaths-value player-stat-value\">" . $stats->deaths . "</span>\n";
	$markup = $markup . "</div>\n";
	
	$markup = $markup . "<div class=\"total-pvp-kills\">\n";
	$markup = $markup . "<span class=\"total-pvp-kills-label player-stat-label\">Total PVP Kills</span>\n";
	$markup = $markup . "<span class=\"total-pvp-kills-value player-stat-value\">" . $stats->pvpKills . "</span>\n";
	$markup = $markup . "</div>\n";
	
	$markup = $markup . "<div class=\"total-pvp-deaths\">\n";
	$markup = $markup . "<span class=\"total-pvp-deaths-label player-stat-label\">Total PVP Deaths</span>\n";
	$markup = $markup . "<span class=\"total-pvp-deaths-value player-stat-value\">" . $stats->pvpDeaths . "</span>\n";
	$markup = $markup . "</div>\n";
	
	return $markup;
}
function _get_api_url() {
	return variable_get ( 'eechat_url', t ( 'http://server.theanubianwar.com/api' ) );
}
function _doHTTP($path, $method = 'GET', $postdata = false) {
	$curl = curl_init ();
	curl_setopt ( $curl, CURLOPT_VERBOSE, TRUE );
	curl_setopt ( $curl, CURLOPT_RETURNTRANSFER, 1 );
	curl_setopt ( $curl, CURLOPT_HEADER, false );
	curl_setopt ( $curl, CURLOPT_AUTOREFERER, TRUE );
	curl_setopt ( $curl, CURLOPT_CONNECTTIMEOUT, 5 );
	
	if ($method === 'GET')
		curl_setopt ( $curl, CURLOPT_CUSTOMREQUEST, 'GET' );
	else {
		$fields_string = "";
		foreach ( $postdata as $key => $value ) {
			$fields_string .= $key . '=' . $value . '&';
		}
		rtrim ( $fields_string, '&' );
		curl_setopt ( $curl, CURLOPT_POST, count ( $postdata ) );
		curl_setopt ( $curl, CURLOPT_POSTFIELDS, $fields_string );
	}
	
	curl_setopt ( $curl, CURLOPT_URL, _get_api_url () . $path );
	
	$data = curl_exec ( $curl );
	$request_headers = ( object ) curl_getinfo ( $curl );
	curl_close ( $curl );
	
	return array (
			$data,
			$request_headers 
	);
}
function _get_race_name($raceid) {
	$races = array (
			'a' => "Anura",
			'v' => "Atavian",
			'd' => "Bandicoon",
			'b' => "Bounder",
			'r' => "Broccan",
			'c' => "Caprican",
			'w' => "Clockwork",
			'p' => "Cyclops",
			'e' => "Daemon",
			'q' => "Dryad",
			'g' => "Fangren",
			'f' => "Feline",
			'x' => "Foxen",
			'h' => "Hart",
			'o' => "Longtail",
			'l' => "Lisian",
			'n' => "Noctari",
			't' => "Taurian",
			's' => "Troblin",
			'k' => "Tusken",
			'u' => "Ursine",
			'y' => "Yeti" 
	);
	
	return $races [$raceid];
}
function _get_rank($rankid) {
	switch ($rankid) {
		case 4 :
			return "Leader";
		case 3 :
			return "Officer";
		case 2 :
			return "Member";
		case 1 :
			return "Initiate";
	}
	return "None";
}
function _get_class_name($classid) {
	$classname = "Unknown";
	switch ($classid) {
		case 1 :
			$classname = "Knight";
			break;
		case 2 :
			$classname = "Rogue";
			break;
		case 3 :
			$classname = "Mage";
			break;
		case 4 :
			$classname = "Druid";
			break;
	}
	return $classname;
}

function _get_weapon_type_name($itemtype) {
	switch ($itemtype) {
		case WeaponType::NONE:
			return "None";
		case WeaponType::SMALL:
			return "Small";
		case WeaponType::ONE_HAND:
			return "One Hand";
		case WeaponType::TWO_HAND:
			return "Two Hand";
		case WeaponType::POLE:
			return "Pole";
		case WeaponType::WAND:
			return "Wand";
		case WeaponType::BOW:
			return "Bow";
		case WeaponType::THROWN:
			return "Thrown";
		case WeaponType::ARCANE_TOTEM:
			return "Totem";
	}
	return "Unknown";
}


function _get_item_type_name($itemtype) {
	switch ($itemtype) {
		case ItemType::BASIC:
			return "Basic";
		case ItemType::SYSTEM:
			return "System";
		case ItemType::ARMOR:
			return "Armor";
		case ItemType::WEAPON:
			return "Weapon";
		case ItemType::CHARM:
			return "Charm";
		case ItemType::CONSUMABLE:
			return "Consumable";
		case ItemType::CONTAINER:
			return "Container";
		case ItemType::BASIC:
			return "Basic";
		case ItemType::SPECIAL:
			return "Special";
		case ItemType::QUEST:
			return "Quest";
		case ItemType::RECIPE:
			return "Recipe";
	}
	return "Unknown";
}
