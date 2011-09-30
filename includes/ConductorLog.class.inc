<?php

abstract class  ConductorLog implements ConductorObserver {

  public function notify($event, object $object) {
    $this->logEvent($event, $object);
  }

  /**
   * @param $event
   *   A string representing the log event.
   */
  abstract public function logEvent($event, object $object);

  /**
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
