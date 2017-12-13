<?php

/**
 * Settings form.
 */
function eechat_settings_form($form, &$form_state) {
    $form['eechat_url_TAW'] = array(
        '#type' => 'textfield',
        '#title' => t('Server API URL'),
        '#size' => 60,
        '#description' => t('The HTTP API URL of the Next Generation Earth Eternal server (TAWD and above). For example http://server.theanubianwar.com/api'),
        '#default_value' => variable_get('eechat_url_TAW', t('http://server.theanubianwar.com/api'))
    );
    
    $form['eechat_url_EETEST'] = array(
        '#type' => 'textfield',
        '#title' => t('EETEST Server API URL'),
        '#size' => 60,
        '#description' => t('The HTTP API URL of the EETEST Earth Eternal server. For example http://legacy.theanubianwar.com/api'),
        '#default_value' => variable_get('eechat_url_EETEST', t('http://legacy.theanubianwar.com'))
    );
    
    $form['eechat_url_VAL'] = array(
        '#type' => 'textfield',
        '#title' => t('VAL Server API URL'),
        '#size' => 60,
        '#description' => t('The HTTP API URL of the VAL Earth Eternal server. For example http://valkal.theanubianwar.com/api'),
        '#default_value' => variable_get('eechat_url_VAL', t('http://valkal.theanubianwar.com'))
    );
    
    $form['eechat_authtoken_VAL'] = array(
        '#type' => 'textfield',
        '#title' => t('VAL Server Auth Token'),
        '#size' => 60,
        '#description' => t('The authentication token used used on the VAL Earth Eternal server'),
        '#default_value' => variable_get('eechat_authtoken_VAL', t('mysecret'))
    );
    
    $form['eechat_authtoken_EETEST'] = array(
        '#type' => 'textfield',
        '#title' => t('EETEST Server Auth Token'),
        '#size' => 60,
        '#description' => t('The authentication token used used on the EETEST Earth Eternal server'),
        '#default_value' => variable_get('eechat_authtoken_EETEST', t('mysecret'))
    );
    
    $form['eechat_key'] = array(
        '#type' => 'textfield',
        '#title' => t('Shroomie Search Encryption Key'),
        '#size' => 60,
        '#description' => t('The encryption key used for encrypting scores.'),
        '#default_value' => variable_get('eechat_key', t(''))
    );

    return system_settings_form($form);
}

/**
 * Settings form validate.
 */
function eechat_settings_form_validate($form, &$form_state) {
}

