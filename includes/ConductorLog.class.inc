<?php

/**
 * Provides a generic logging capability for Conductor Workflow processing.
 *
 * Objects implementing the ConductorObserver interface can be registered with
 * a workflow's state object to oberve events happening in the processing of that
 * workflow.  This abstract class generates log messages from the events such that
 * an extending class may implement a logEvent() method for logging to some specific
 * output.
 */
abstract class  ConductorLog implements ConductorObserver {

  public $logThreshold;

  /**
   * Set the level of log entry that this logger should log.
   *
   * (If a ConductorLogger could log logs.)
   */
  public function __construct($logThreshold = 0) {
    $this->logThreshold = $logThreshold;
  }

  /**
   * Implements ConductorObserver::notify().
   */
  public function notify($event, $object) {
    $logLevel = $this->deriveLogLevel($event);
    $this->logEvent($event, $object, $logLevel);
  }

  /**
   * Log an event that has occured.
   *
   * @param $event
   *   A string representing the log event.
   * @param $object
   *   The object that the event most directly happpened to.
   * @param $logLevel
   *   An integer representing the level.
   *   0 is normal, less than zero is minor (only with verbose
   *   options), greater indicates a problem.
   */
  abstract public function logEvent($event, $object, $logLevel);

  /**
   * @param $event
   *   A string representing the name of the event that occured.
   * @param $object
   *   The object that the event most directly happpened to.
   * @return
   *   A string representing a human readable log friendly message.
   */
  public function generateHumanMessage($event, $object) {
    switch ($event) {
      case 'activityActivate':
        $string = 'Event @event was activated';
        $args = array('@event' => $object->title);
        break;
      case 'activityDeactivate':
        $string = 'Event @event was activated';
        $args = array('@event' => $object->title);
        break;
      case 'activityComplete':
        $string = 'Event @event was successfully completed.';
        $args = array('@event' => $object->title);
        break;
      case 'activitySuspend':
        $string = 'Event @event was suspended, awaiting further action.';
        $args = array('@event' => $object->title);
        break;
      case 'activityResume':
        $string = 'Event @event has been resumed.';
        $args = array('@event' => $object->title);
        break;
      case 'activityFail':
        $string = 'Event @event has failed.';
        $args = array('@event' => $object->title);
        break;
      default:
        $string = t('Conductor just @evented.');
        $args = array('@event' => $event);
        break;
    }
    return array( $string,  $args);
  }

  /**
   * Derive a logLevel to indicate the severity of the message.
   *
   * @param $event
   *   A string representing the name of the event that occured.
   * @return
   *   An integer representing the level.
   *   0 is normal, less than zero is minor (only with verbose
   *   options), greater indicates a problem.
   */
  public function deriveLogLevel($event) {
    $error = array();
    $minor = array(
      'activateActivity',
    );
    if (in_array($event, $minor)) {
      $level = -1;
    }
    else if (in_array($event, $error)) {
      $level = 0;
    }
    else {
      $level = 0;
    }
    return $level;
  }

}
