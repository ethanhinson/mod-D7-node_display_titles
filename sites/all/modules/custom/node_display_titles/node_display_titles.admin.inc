<?php
/**
 * Provides form for addin context name for Title override
 */
function node_display_titles_config($form, &$form_state) {
  $types = node_type_get_types();
  $entity_info = entity_get_info();

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
    // Create a fieldset
    $form['node_display_titles_node_types']['node_' . $type->type] = array(
      '#title' => $type->name,
      '#type' => 'fieldset',
    );

    $form['node_display_titles_node_types']['node_' . $type->type][$type->type . '_title_pattern'] = array(
      '#title' => 'Pattern',
      '#type' => 'textfield',
      '#attributes' => array(),
      '#default_value' => variable_get($type->type . '_title_pattern', ''),
      '#group' => 'node_display_titles',
    );

    $node_entity_info = $entity_info['node'];

    // If there are any view modes enabled for this node type, create a set of
    // checkboxes to apply the pattern per layout.
    if (!empty($node_entity_info['bundles'][$type->type]['layouts'])) {

      // Create a collapsed fieldset
      $form['node_display_titles_node_types']['node_' . $type->type][$type->type . '_viewmodes'] = array(
        '#title' => 'View Modes',
        '#type' => 'fieldset',
        '#collapsible' => 'collapsible',
        '#collapsed' => 'collapsed',
      );

      // Create a checkbox for each enabled layout
      foreach ($node_entity_info['bundles'][$type->type]['layouts'] as $layout => $layout_settings) {
        $layout_key = $type->type . '_' . $layout . '_node_display_titles_layout';


        // Get a human readable label for each view mode.
        $label = $node_entity_info['view modes'][$layout]['label'];
        if (!$label) {
          $label = 'Default';
        }

        $form['node_display_titles_node_types']['node_' . $type->type][$type->type . '_viewmodes'][$layout_key] = array(
          '#title' => $label,
          '#type' => 'checkbox',
          '#attributes' => array(),
          '#default_value' => variable_get($layout_key, ''),
          '#group' => 'node_display_titles',
        );
      }
    } else {
      $form['node_display_titles_node_types']['entity_' . $type->type][$type->type . '_viewmodes'] = array(
        '#markup' => '<div class="description">No view modes enabled for ' . $type->name . '</div>',
      );
    }
  }

  $form['node_display_titles_node_types']['tokens'] = array(
    '#theme' => 'token_tree',
    '#token_types' => 'all', // The token types that have specific context. Can be multiple token types like 'term' and/or 'user'
    '#global_types' => TRUE, // A boolean TRUE or FALSE whether to include 'global' context tokens like [current-user:*] or [site:*]. Defaults to TRUE.
    '#click_insert' => TRUE, // A boolean whether to include the 'Click this token to insert in into the the focused textfield' JavaScript functionality. Defaults to TRUE.
  );

  // Entity settings
  $form['node_display_titles_entities'] = array(
    '#type' => 'fieldset',
    '#title' => t('Entity Patterns'),
    '#collapsible' => TRUE,
    '#group' => 'node_display_titles',
    '#description' => t('<p>Enter patterns to use per entity. This will override the H1 display for full nodes. HTML markup is allowed.</p>'),
  );

  foreach ($entity_info as $entity_type => $info) {
    $form['node_display_titles_entities']['entity_' . $entity_type] = array(
      '#title' => $info['label'],
      '#type' => 'fieldset',
    );

    $form['node_display_titles_entities']['entity_' . $entity_type]['entity_' . $entity_type . '_title_pattern'] = array(
      '#title' => 'Pattern',
      '#type' => 'textfield',
      '#attributes' => array(),
      '#default_value' => variable_get('entity_' . $entity_type . '_title_pattern', ''),
      '#group' => 'node_display_titles_entities',
    );

    if (count($info['view modes'])) {
      // Entity settings
      $form['node_display_titles_entities']['entity_' . $entity_type][$entity_type] = array(
        '#type' => 'fieldset',
        '#title' => t('View Modes'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,

        //'#group' => 'node_display_titles_entities',
        //'#description' => t('<p>Enter patterns to use per entity. This will override the H1 display for full nodes. HTML markup is allowed.</p>'),
      );

      foreach ($info['view modes'] as $layout => $view_mode) {
        $layout_key = 'entity_' . $entity_type . '_' . $layout . '_node_display_titles_layout';

        $form['node_display_titles_entities']['entity_' . $entity_type][$entity_type][$layout_key] = array(
          '#title' => $view_mode['label'],
          '#type' => 'checkbox',
          '#attributes' => array(),
          '#default_value' => variable_get($layout_key, ''),
          '#group' => 'node_display_titles_entities',
        );
      }
    } else {
      $form['node_display_titles_entities']['entity_' . $entity_type][$entity_type]['viewmodes'] = array(
        '#type' => 'markup',
        '#markup' => '<div class="description">' . t($info['label']) . ': <em>No view modes enabled.</em></div>',
        '#group' => 'node_display_titles_entities',
      );
    }
  }


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