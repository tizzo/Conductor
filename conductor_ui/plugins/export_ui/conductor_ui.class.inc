<?php

/**
 * @file
 * Contains the CTools Export UI integration code.
 */

/**
 * CTools Export UI class handler for Conductor UI.
 */
class conductor_ui extends ctools_export_ui {

  /**
   * Implements ctools_export_ui::list_page().
   */
  function list_page($js, $input) {
    conductor_ui_add_css();
    // wrap output in a div for CSS
    $output = parent::list_page($js, $input);
    if (is_string($output)) {
      $output = '<div id="conductor-ui-list-page">' . $output . '</div>';
      return $output;
    }
  }

  /**
   * Implements ctools_export_ui::clone_page().
   */
  /*
   * TODO: Properly implement this!
  function clone_page($js, $input, $item, $step = NULL) {
    drupal_set_title($this->get_page_title('clone', $item));

    $name = $item->{$this->plugin['export']['key']};

    $form_state = array(
      'plugin' => $this->plugin,
      'object' => &$this,
      'ajax' => $js,
      'item' => $item,
      'op' => 'add',
      'form type' => 'clone',
      'original name' => $name,
      'rerender' => TRUE,
      'no_redirect' => TRUE,
      'step' => $step,
      // Store these in case additional args are needed.
      'function args' => func_get_args(),
    );

    $output = drupal_build_form('conductor_ui_clone_form', $form_state);
    if (!empty($form_state['executed'])) {
      $item->name = $form_state['values']['name'];
      $item->human_name = $form_state['values']['human_name'];
      $item->vid = NULL;
      conductor_ui_cache_set($item);

      //drupal_goto(ctools_export_ui_plugin_menu_path($this->plugin, 'edit', $item->name));
    }

    return $output;
  }
  */

  /**
   * Implements ctools_export_ui::edit_page().
   *
   * Main entry point to edit an item.
   */
  function edit_page($js, $input, $workflow, $step = NULL) {

    conductor_ui_set_breadcrumb();
    conductor_ui_add_js();
    conductor_ui_add_css();
    ctools_include('modal');
    ctools_include('object-cache');
    ctools_modal_add_js();
    conductor_ui_add_ctools_modal_style();

    $output = array();
    $output['workflow_element_panel'] = array(
      '#prefix' => '<div id="conductor-ui-toolbar">',
      '#suffix' => '</div>',
    );
    /*
    $output['workflow_element_panel']['add_activity'] = array(
      '#type' => 'markup',
      '#markup' => ctools_modal_text_button(t('+ Add Activity'), 'conductor_ui/nojs/activity', t('Add an actiivty to this workflow.')),
    );
    //*/
    $output['workflow_element_panel']['buttons'] = array(
      '#prefix' => '<div id="toolbar-buttons">',
      '#suffix' => '</div>',
    );
    $output['workflow_element_panel']['buttons']['save'] = array(
      '#value' => t('Save'),
      '#type' => 'button',
    );
    $output['workflow_element_panel']['buttons']['cancel'] = array(
      '#value' => t('Cancel'),
      '#type' => 'button',
    );
    $output['workflow_editor'] = array(
      '#prefix' => '<div id="conductor-workflow-editor">',
      '#suffix' => '</div>',
    );
    $activities = $workflow->getActivities(); 
    $settings = array();
    foreach ($activities as $name => $activity) {
      // Prevent us from making changes to this activity during render.
      $class_name = 'conductor-ui-activity-' . conductor_ui_css_safe_class($activity->name);
      $activity->css_class = $class_name;
      $drop_links = theme('links__ctools_dropbutton', array('title' => t('Edit'), 'links' => $activity->getUILinks()));
      $output['workflow_editor'][$name] = array(
        '#name' => $activity->name,
        '#title' => $activity->title,
        '#class' => $class_name,
        '#drop_links' => $drop_links,
        '#theme' => 'conductor_activity',
        '#contextual_links' => array(
          'contextual_example' => array( 'contextual', array('foobars')),
        ),
      );
      // Clone the activity to prevent changes from effecting the original object.
      $settings_activity = clone $activity;
      // Unset the workflow object handle.
      unset($activity->workflow);
      // Add the activity to the settings array so that it can be used later.
      $settings['activities'][$name] = $activity;
    }
    drupal_add_js(array('conductor_ui' => $settings), array('type' => 'setting'));
    return $output;
  }
}


/**
 * Form callback to edit an exportable item using the wizard
 *
 * This simply loads the object defined in the plugin and hands it off.
 */
function conductor_ui_clone_form($form, &$form_state) {
  $counter = 1;

  if (!isset($form_state['item'])) {
    $workflow = conductor_load_workflow($form_state['original name']);
  }
  else {
    $workflow = $form_state['item'];
  }
  do {
    if (empty($form_state['item']->is_template)) {
      $name = format_plural($counter, 'Clone of', 'Clone @count of') . ' ' . $workflow->name;
    }
    else {
      $name = $workflow->name;
      if ($counter > 1) {
        $name .= ' ' . $counter;
      }
    }
    $counter++;
    $machine_name = preg_replace('/[^a-z0-9_]+/', '_', drupal_strtolower($name));
  } while (ctools_export_crud_load($form_state['plugin']['schema'], $machine_name));

  $form['human_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Workflow name'),
    '#default_value' => $name,
    '#size' => 32,
    '#maxlength' => 255,
  );

  $form['name'] = array(
    '#title' => t('Workflow name'),
    '#type' => 'machine_name',
    '#required' => TRUE,
    '#maxlength' => 32,
    '#size' => 32,
    '#machine_name' => array(
      'exists' => 'ctools_export_ui_edit_name_exists',
      'source' => array('human_name'),
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Continue'),
  );

  return $form;
}

