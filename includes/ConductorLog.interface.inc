<?php

abstract class  ConductorLog implements ConductorObserver {

  /**
   * An integer representing the severity of messages to log.
   *
   * TODO: Verify that values here are consistent with those in ConductorActivityState.
   */
  public $logThreshold = 0;

  /**
   * Recieve a notification about an event that occured in a workflow instance.
   *
   * @param $event
   *   A string representing the name of the event that occured.
   * @return
   *   An integer representing the level.
   *   0 is normal, less than zero is minor (only with verbose
   *   options), greater indicates a problem.
   */
  public function notify($event, object $object) {
    // TODO: This piece of indirection is probably  unnecessary,
    // we should probably just use notify directly.
    $this->logEvent($event, $object);
  }

  /**
   * @param $event
   *   A string representing the log event.
   */
  abstract public function logEvent($event, object $object);

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
    switch ($event) {
      case 'default':
        $level = 0;
        break;
    }
    return $level;
  }

  /**
   * Generate a human readable string representing an event.
   *
   * @param $event
   *   A string representing the name of the event that occured.
   * @param $object
   *   The object that the event most directly happpened to.
   * @return
   *   A string representing a human readable log friendly message.
   */
  public function generateHumanMessage($event, object $object) {
    // TODO: make this smarter.
    switch ($event) {
      case 'activateEvent':
        $message = t('Event @event was activated', array('@event' => $object->title));
        break;
      case '':
      case default:
        $message = t('Conductor just @evented.', array('@event' => $event));
        break;
    }
    return $message;
  }


}
