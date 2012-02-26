<?php

/**
 * @file
 *   Documents the conductor API.
 */

/**
 * Implements hook_conductor_activity_process().
 *
 * Allows modules to modify the
 *
 * @param $plugin
 *   The ctools plugin data.
 */
function hook_conductor_activity_process(&$plugin) {
  $plugin += array(
    'foo' => 'bar',
  );
}

/**
 * Usually workflow component objects are setup automatically, here is an example of
 * how to explicitly set the handlers in a workflow.
 */
function example_manual_workflow_factory() {
  $workflow = $this->getWorkflow($name);
  // Instantiate the workflow instance for processing.
  $instance = new ConductorInstance();
  // Instantiate our mock storage handler.
  $storage = new conductorStorageTest;
  // Provide our mock storage handler with an array to use as a persistent
  // data store.
  $storage->setDatabase($this->dataStore);
  // Set the storage handler for this instance.
  $instance->setStorage($storage);
  // Actually assign the workflow instance state to the workflow.
  $workflow->setState($instance);
  // Create the context
  $context = array('foo' => 'bar');
  // Get the first activity, we will need it to set the initial context.
  $start = $workflow->getActivity('start');
  // Instantiate the workflow instance for processing.
  $instance = new ConductorInstance();
  // Instantiate our mock storage handler.
  $storage->setDatabase($this->dataStore);
  // Set the storage handler for this instance.
  $instance->setStorage($storage);
  // Create a state object for the first activity.
  $startState = new ConductorActivityState($start, $instance);
  // Set the context.
  $startState->context = $context;
  // Add the activity's state to the workflow instance state.
  $instance->setActivityStates(array('start' => $startState));
  // Actually assign the workflow instance state to the workflow.
  $workflow->setState($instance);
  // Set up the observer for tracking workflow history in tests.
  $observer = new ConductorObserverTest;
  $observer->history = &$history;
  $workflow->state->registerObserver($observer);

  }
