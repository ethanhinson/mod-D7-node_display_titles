<?php
/**
 * Provides form for addin context name for Title override
 */
function node_display_titles_config($form, &$form_state) {
  $entity_info = entity_get_info();

  $form['node_display_titles'] = array(
    '#type' => 'vertical_tabs',
  );

  // Node type patterns and tokens
  $form['node_display_titles_node_types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content Type Patterns'),
    '#collapsible' => TRUE,
    '#group' => 'node_display_titles',
    '#description' => t('<p>Enter patterns to use per content type. This will override the H1 display for full nodes. HTML markup is allowed.</p>'),
  );

  // Generate pattern field and settings for each node type.
  foreach($entity_info['node']['bundles'] as $type => $bundle) {
    // Generate an array of viewmode human readable names indexed by viewmode machine name.
    $layouts = array();
    if (!empty($bundle['layouts'])) {
      $layout_keys = array_keys($bundle['layouts']);
      foreach ($layout_keys as $layout) {
        // Get a human readable label for this view mode.
        if (isset($entity_info['node']['view modes'][$layout]['label'])) {
          $label = $entity_info['node']['view modes'][$layout]['label'];
        }

        if (!$label) {
          $label = 'Default';
        }

        $layouts[$layout] = $label;
      }
    }

    // Generate a tab for node type patterns.
    $form['node_display_titles_node_types'][$type] = _node_display_titles_tab($bundle['label'], $type, $layouts);
  }

  // Add token menu to choose from
  $form['node_display_titles_node_types']['tokens'] = array(
    '#theme' => 'token_tree',
    '#token_types' => 'all', // The token types that have specific context. Can be multiple token types like 'term' and/or 'user'
    '#global_types' => TRUE, // A boolean TRUE or FALSE whether to include 'global' context tokens like [current-user:*] or [site:*]. Defaults to TRUE.
    '#click_insert' => TRUE, // A boolean whether to include the 'Click this token to insert in into the the focused textfield' JavaScript functionality. Defaults to TRUE.
  );

  // Entity patterns and tokens.
  $form['node_display_titles_entities'] = array(
    '#type' => 'fieldset',
    '#title' => t('Entity Patterns'),
    '#collapsible' => TRUE,
    '#group' => 'node_display_titles',
    '#description' => t('<p>Enter patterns to use per entity. This will override the H1 display in every view mode for which it is enabled. HTML markup is allowed.</p>'),
  );

  $field_info = field_info_field_map();

  // Generate pattern field and settings for each entity type.
  foreach ($entity_info as $entity_type => $info) {
    // Generate a list of entity fields that can be overridden
    $options = array();
    foreach ($field_info as $name => $field_info_details) {
      if (array_key_exists($entity_type, $field_info_details['bundles'])) {
        $options[$name] = $name;
      }
    }

    // Generate an array of viewmode human readable names indexed by viewmode machine name for this entity type.
    $layouts = array();
    if (!empty($info['view modes'])) {
      foreach ($info['view modes'] as $layout => $view_mode) {
        $layouts[$layout] = $view_mode['label'];
      }
    }

    $form['node_display_titles_entities'][$entity_type] = _node_display_titles_tab($info['label'], 'entity_' . $entity_type,  $layouts, $options);
  }

  // Add token menu to choose from
  $form['node_display_titles_entities']['tokens'] = array(
    '#theme' => 'token_tree',
    '#token_types' => 'all', // The token types that have specific context. Can be multiple token types like 'term' and/or 'user'
    '#global_types' => TRUE, // A boolean TRUE or FALSE whether to include 'global' context tokens like [current-user:*] or [site:*]. Defaults to TRUE.
    '#click_insert' => TRUE, // A boolean whether to include the 'Click this token to insert in into the the focused textfield' JavaScript functionality. Defaults to TRUE.
  );

  // Title override settings
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

/**
 * @param $label
 * @param $type
 * @param $layouts
 * @return array
 *
 * Generate a tab with title patterns and viewmode checkboxes.
 */
function _node_display_titles_tab($label, $type, $layouts, $options = array()) {
  // Create a fieldset for this type's settings
  $fieldset = array(
    '#title' => $label,
    '#type' => 'fieldset',
  );

  // Create a pattern textfield for this type's title pattern
  $fieldset[$type . '_title_pattern'] = array(
    '#title' => 'Pattern',
    '#type' => 'textfield',
    '#attributes' => array(),
    '#default_value' => variable_get($type . '_title_pattern', ''),
    '#group' => 'node_display_titles',
    '#maxlength' => 350,
  );

  if (count($options)) {
    // Alphabetize override field dropdown options
    ksort($options);

    // Create a dropdown to select field to override for this entity type
    $fieldset[$type . '_override_field'] = array(
      '#title' => 'Field to Override',
      '#type' => 'select',
      '#attributes' => array(),
      '#default_value' => variable_get($type . '_override_field', ''),
      '#group' => 'node_display_titles',
      '#maxlength' => 350,
      '#options' => $options,
    );
  }

  if (empty($layouts)) {
    // Generate a message indicating there are no view modes for this type.
    $fieldset['layouts'] = array('#markup' => '<div class="description">There are no view modes enabled for ' . $label . '.</div>');
  }
  else {
    // Create a collapsed fieldset of view mode checkboxes
    $fieldset[$type . '_viewmodes'] = array(
      '#title' => 'View Modes',
      '#type' => 'fieldset',
      '#collapsible' => 'collapsible',
      '#collapsed' => 'collapsed',
    );

    // Create a checkbox for each enabled layout
    foreach ($layouts as $layout => $label) {
      $layout_key = $type . '_' . $layout . '_node_display_titles_layout';

      $fieldset[$type . '_viewmodes'][$layout_key] = array(
        '#title' => $label,
        '#type' => 'checkbox',
        '#attributes' => array(),
        '#default_value' => variable_get($layout_key, ''),
        '#group' => 'node_display_titles',
      );
    }
  }

  return $fieldset;
}
