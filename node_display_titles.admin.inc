<?php
/**
 * Provides form for addin context name for Title override
 */
function node_display_titles_config($form, &$form_state) {
  $types = node_type_get_types();
  $form['node_display_titles'] = array(
    '#type' => 'vertical_tabs',
  );

  /**
   * Node type patterns and tokens
   */

  $form['node_display_titles_node_types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content Type Patterns'),
    '#collapsible' => TRUE,
    '#group' => 'node_display_titles',
    '#description' => t('<p>Enter patterns to use per content type. This will override the H1 display for full nodes. HTML markup is allowed.</p>'),
  );
  foreach($types as $type) {
    $form['node_display_titles_node_types'][$type->type . '_title_pattern'] = array(
      '#title' => $type->name . ' Pattern',
      '#type' => 'textfield',
      '#attributes' => array(),
      '#default_value' => variable_get($type->type . '_title_pattern', ''),
      '#group' => 'node_display_titles',
    );
  }

  $form['node_display_titles_node_types']['tokens'] = array(
    '#theme' => 'token_tree',
    '#token_types' => 'all', // The token types that have specific context. Can be multiple token types like 'term' and/or 'user'
    '#global_types' => TRUE, // A boolean TRUE or FALSE whether to include 'global' context tokens like [current-user:*] or [site:*]. Defaults to TRUE.
    '#click_insert' => TRUE, // A boolean whether to include the 'Click this token to insert in into the the focused textfield' JavaScript functionality. Defaults to TRUE.
  );

  /**
   * Title override settings
   */

  $form['node_display_titles_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#collapsible' => TRUE,
    '#group' => 'node_display_titles',
  );

  $form['node_display_titles_settings']['node_display_titles_sanitization_callback'] = array(
    '#type' => 'textfield',
    '#title' => t('Sanitization Callback'),
    '#group' => 'node_display_titles',
    '#description' => t('Name of a callback function that will be used to sanitize token data. The current callback simply strips all markup from tokens'),
    '#default_value' => variable_get('node_display_titles_sanitization_callback', 'node_display_titles_token_sanitize'),
  );

  $form['node_display_titles_settings']['node_display_titles_clear_tokens'] = array(
    '#type' => 'checkbox',
    '#title' => t('Clear empty Tokens'),
    '#group' => 'node_display_titles',
    '#description' => t('Remove token text when no valid replacement is found.'),
    '#default_value' => variable_get('node_display_titles_clear_tokens', TRUE),
  );

  return system_settings_form($form);
}