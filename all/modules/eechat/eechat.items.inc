<?php

function eechat_tooltip_item($id = "1") {
	$params = $_POST;
	drupal_add_http_header ( 'Content-Type', 'text/html' );
	print "<html>\r\n";
	print "<body>\r\n";

	$urlp = "/item/" . urlencode ( $id );
	list ( $data, $request_headers ) = _doHTTP ( $urlp );
	print "<div class=\"item\">\n";
	if (isset ( $request_headers->http_code ) && $request_headers->http_code == 0) {
		$markup = $markup . t ( 'Earth Eternal is OFFLINE' );
	} else if (isset ( $request_headers->http_code ) && $request_headers->http_code < 300 && $request_headers->http_code >= 200) {
		$itemdata = json_decode ( $data );
		switch($itemdata->type) {
			case ItemType::UNKNOWN:
			case ItemType::BASIC:
			case ItemType::SYSTEM:
				_render_name($itemdata);
				_render_binding_info($itemdata);
				break;
			default:
				print "<h3>Unsupported type " . $itemdata->type . "</h3>\r\n";
				_render_name($itemdata);
				break;
		}
		
		//print "<h2>Level " . $itemdata->level . "</h2>";
	}
	print "</div>\r\n";
	
	print "</body>\r\n";
	print "</html>\r\n";
	
	exit ( 0 );
}

function _render_binding_info($itemdata) {	
	print "<div class=\"binding-info\">" . $BINDING_TYPE_NAMES[$itemdata->bindingType] . "</div>";
}

function _render_name($itemdata) {	
	print "<h1>" . $itemdata->displayName . "</h1>";
}

?>