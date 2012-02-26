<?php

/**
 *
 */
function conductor_test_default_conductor_workflows() {
  $workflows = array();
  $workflow = new ConductorWorkflow;
  $workflow->wid = 'new';
  $workflow->name = 'simple_parallel_activity_test';
  $workflow->title = 'Simple Parallel activity';
  $workflow->description = 'A simple activity that executes 2 activities in parallel.';
  $workflow->api_version = '1.0';
  // Add and configure workflow activities.
  $activity = $workflow->newActivity('start');
  $activity->x = 280;
  $activity->y = 200;
  $activity->name = 'start';
  $activity->title = 'Start';
  $activity->outputs = array(
    'activity_2',
    'activity_3'
  );
  $activity->inputs = array();
  // Implement a new test for pausing, we shouldn't reuse the current test.
  //$activity = $workflow->newActivity('pauseTest', $this->getActivity('tests/plugins/activity/pause_test/pause_test.inc'));
  $activity = $workflow->newActivity('activity');
  $activity->x = 180;
  $activity->y = 100;
  $activity->name = 'activity_2';
  $activity->title = 'Activity 2';
  $activity->inputs = array(
    'start',
  );
  $activity->outputs = array(
    'end'
  );
  $activity = $workflow->newActivity('activity');
  $activity->x = 180;
  $activity->y = 100;
  $activity->name = 'activity_3';
  $activity->title = 'Activity 3';
  $activity->inputs = array(
    'start',
  );
  $activity->outputs = array(
    'end'
  );
  $activity = $workflow->newActivity('end');
  $activity->x = 380;
  $activity->y = 200;
  $activity->name = 'end';
  $activity->title = 'End';
  $activity->inputs = array(
    'activity_2',
    'activity_3'
  );
  $activity->outputs = array();
  $workflows[$workflow->name] = $workflow;
  return $workflows;
}

