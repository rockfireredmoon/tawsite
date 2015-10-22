<?php

/**
 * TAWD - Earth Eternal The Anubian War provider adapter based on OAuth2 protocol
 */
class Hybrid_Providers_Tawd extends Hybrid_Provider_Model_OAuth2
{
    /**
     * Initializer
     */
    function initialize()
    {
        parent::initialize();

        $this->api->api_base_url = 'http://192.168.91.4:9052/';
        $this->api->authorize_url = 'http://192.168.91.4:9052/oauth/authorize';
        $this->api->token_url = 'http://192.168.91.4:9052/oauth/token';
    }
    
    /**
	* load the user profile from the IDp api client
	*/
	function getUserProfile(){
		$data = $this->api->api("oauth/self" ); 

		if ( $this->api->http_code != 200 ){
			throw new Exception( "User profile request failed! {$this->providerId} returned an invalid response.", 6 );
		}

		$this->user->profile->identifier  = $data->data->id; 
		$this->user->profile->displayName = $data->data->username; 
		$this->user->profile->username    = $data->data->username;
		
		//$this->user->profile->emailVerified = "veteran@planetforevergame.com";
		
// 		try {
// 			$this->user->roles = array(
// 					8 => 'veterans'
// 			);
// 		}
// 		catch(Exception $e) {
// 			file_put_contents('php://stderr', print_r(e, TRUE));
// 		}
		/*$this->user->roles = array();
		array_push($this->user->roles, "veterans");*/

		return $this->user->profile;
	}
    
    /**
     * load the user profile from the IDp api client
     */
    /*function getUserProfile(){
    	$data = $this->api->api("users/self/" );
    
    	if ( $data->meta->code != 200 ){
    		throw new Exception( "User profile request failed! {$this->providerId} returned an invalid response.", 6 );
    	}
    
    	$this->user->profile->identifier  = $data->data->id;
    	$this->user->profile->displayName = $data->data->full_name ? $data->data->full_name : $data->data->username;
    	$this->user->profile->description = $data->data->bio;
    	$this->user->profile->photoURL    = $data->data->profile_picture;
    
    	$this->user->profile->webSiteURL  = $data->data->website;
    
    	$this->user->profile->username    = $data->data->username;
    
    	return $this->user->profile;
    }*/

    /**
     * Begin login step
     */
    /*
    function loginBegin()
    {
        // redirect the user to the provider authentication url
        Hybrid_Auth::redirect($this->api->authorizeUrl());
    }
    */

    /**
     * Request method with Bearer access_token auth
     * @param $url
     * @return mixed
     */
    
    /*
    function request($url, $params = null)
    {
        $ch = curl_init();

        if ($params)
        {
            $url = $url . ( strpos( $url, '?' ) ? '&' : '?' ) . http_build_query($params, '', '&');
        }

        curl_setopt($ch, CURLOPT_HEADER, 0);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_USERAGENT, $this->api->curl_useragent);
        curl_setopt($ch, CURLOPT_URL, $this->api->api_base_url . 'v1/' . $url);
        $headers = array('Authorization: Bearer ' . $this->api->access_token);
        curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
        $data = curl_exec($ch);
        curl_close($ch);

        $data = json_decode($data, true);
        return $data;
    }
    */

    /**
     * Returns artists from library
     * @return null|string
    function getArtists()
    {
        $result = $this->request('me/library/artists');
        return $result;
    }
     */
}
