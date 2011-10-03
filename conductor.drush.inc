<?php
/**
 * @file
 * Drush integration for the conductor module.
 */

/**
 * Implements hook_drush_command().
 */
function conductor_drush_command() {
  $items = array();
  $items['conductor-start'] = array(
    'aliases' => array('cr'),
    'description' => dt('Begins a conductor workflow.'),
    'arguments' => array(
      'name' => dt('The name of the workflow to load and run.'),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
  );
  $items['conductor-list'] = array(
    'aliases' => array('cl'),
    'description' => dt('Begins a conductor workflow.'),
    'arguments' => array(
      'name' => dt('The name of the workflow to load and run.'),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
  );
  return $items;
}

/**
 * Implements hook_drush_help().
 */
function conductor_drush_help($section) {
  switch ($section) {
    case 'drush:conductor-start-workflow':
      return dt("Begins a conductor workflow.");
    case 'drush:conductor-list-workflows':
      return dt("Lists all conductor workflows.");
  }
}



/**
 * Provides a list of the workflows present on a system.
 */
function drush_conductor_list() {
  ctools_include('export');
  $items = ctools_export_crud_load_all('conductor_workflow');
  $rows = array(
    array(
      dt('Title'),
      dt('Name'),
      dt('Description'),
      dt('Storage'),
    ),
  );
  foreach ($items as $workflow) {
    $rows[] = array(
      'title' => $workflow->title,
      'name' => $workflow->name,
      'description' => $workflow->description,
      'storage' => $workflow->type,
    );
  }
  drush_print_table($rows);
  return 'TRUE';
}

/**
 * Creates and runs a new instance of a specified workflow.
 *
 * @param $name
 *   The machine readible name for the workflow.
 */
function drush_conductor_start($name = NULL) {
  if ($name === NULL) {
    drush_set_error('CONDUCTOR_WORKFLOW_NOT_SPECIFIED', dt('Please specify a workflow by machine name.'));
    $success = FALSE;
  }
  elseif (conductor_get_workflow($name) == FALSE) {
    drush_set_error('CONDUCTOR_WORKFLOW_NOT_FOUND', dt('Workflow @workflow not found.', array('@workflow' => $name)));
    $success = FALSE;
  }
  else {
    // TODO: Make default logger registration in some way configurable.
    $options['observers'] = array(
      new ConductorLogDrush(-1),
      new ConductorLogWatchdog(-1)
    );
    $success = conductor_start_workflow($name, $options);
  }
  return $success;
}


