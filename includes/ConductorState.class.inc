<?php

/**
 * This class maintains the state of a Conductor workflow.
 *
 * ConductorState is handed into a ConductorWorkflow to track the state of a specific
 * instance of that workflow.  It delegates saving to a member object stored in the
 * $storageHandler attribute, which must implement the ConductorStorage interface.
 *
 * ConductorState is comprised of ConductorActivityStates.  These 
 */
class ConductorState {

  /**
   * An array of CondcutorActivityState descended object.
   *
   * Each of these objects tracks the state for a specific activity in the
   * workflow.
   */
  protected $activityStates = array();

  /**
   * An array of active activities.
   */
  protected $activeActivities = array();

  /**
   * An array of completed activities.
   */
  protected $completedActivities = array();

  /**
   * A reference to the parent workflow.
   */
  public $workflow = NULL;

  /**
   * An object implementing the ConductorStorage interface.
   *
   * All state CRUD operations are delgated to this object.
   */
  public $storageHandler = NULL;

  //public __construct($activityStates = array(), ConductorStorage $storageHandler) {
  public function __construct($activityStates = array()) {
    $this->activityStates = $activityStates;
  }

  /**
   * Set the activity states.
   */
  public function setActivityStates($activityStates) {
    $this->activityStates = $activityStates;
  }

  public function getActivityState($activityState = NULL) {
    $return = FALSE;

    if (is_null($activityState)) {
      $return = $this->activityStates;
    }
    else if(isset($activityState[$activityState])) {
       $return = $activityState[$activityState];
    }

    return $return;
  }

  public function getIntanceID() {

    return "TODO: we need to generate unique ID's here.  The specifics will vary depending on the state class.  Perhaps we do need plugins for this object?";
  }

  /**
   * Retrieve the current active activities.
   *
   * @return
   *   A linear array of active activities.
   *   
   */
  public function getActiveActivities() {
    // If we do not yet have any activeActivities and we do not yet have any completed
    // activites, we must be starting a workflow.  Run the `start` activity.
    if (!count($this->activeActivities) && !count($this->completeledActivities)) {
      $this->activeActivities[] = 'start';
      $activity = $this->workflow->getActivity('start');
    }
    $state = $this->getActivityStateHandler($activity);
    $this->activateActivity($activity, $state);
    return $this->activeActivities;
  }

  /**
   * Activate a specified activity.
   *
   * @param $activity
   *   The ConductorActivity that has successfully been activated.
   * @param $state
   *   An ConductorActivityState object represeting the state of the present activity.
   */
  public function activateActivity(ConductorActivity $activity, ConductorActivityState $state) {
    $this->activeActivities[$activity->name] = $this->getActivityStateHandler($activity);
  }

  /**
   * Complete a specified activity.
   *
   * @param $activity
   *   The ConductorActivity that has successfully been run and reported finished and should be marked complete.
   * @param $state
   *   An ConductorActivityState object represeting the state of the present activity.
   */
  public function completeActivity(ConductorActivity $activity, ConductorActivityState $state) {
    $this->completedActivities[$activity->name] = &$state;
    foreach ($activity->getOutputs() as $activity_name) {
      $activity = $this->workflow->getActivity($activity_name);
      $this->activateActivity($activity, $this->getActivityStateHandler($activity));
    }
    unset($this->activeActivities[$activity->name]);
  }

  /**
   *
   */
  public function getActivityStateHandler(ConductorActivity $activity) {
    // TODO: Figure out what we need to do to get the Conductor Activity State handler.
    return new ConductorActivityState($activity);
    
  }

}
