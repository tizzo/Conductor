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
   * The workflow has not yet been initialized with state.
   */
  const NO_STATE = 0;

  /**
   * The workflow has been initialized with state but not yet processed.
   */
  const INITIALIZED = 1;

  /**
   * The workflow instance is in the process of being run.
   */
  const RUNNING = 2;

  /**
   * The workflow has finished running.
   */
  const FINISHED = 3;

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

  public $status = self::RUNNING;


  /**
   * Constructor for ConductorState.
   *
   * @param $activityStates (optional)
   *   An associative array of ConductorActivityState objects keyed by $activity->name.
   */
  public function __construct(array $activityStates = array()) {
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
    if (!count($this->activeActivities) && !count($this->completedActivities)) {
      $activity = $this->workflow->getActivity('start');
      $state = $this->getActivityStateHandler($activity);
      $this->activateActivity($activity, $state);
    }
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
    $this->activityStates[$activity->name] = $this->getActivityStateHandler($activity);
    $this->activeActivities[$activity->name] = &$this->activityStates[$activity->name];
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
    foreach ($activity->outputs as $activity_name) {
      $activateActivity = $this->workflow->getActivity($activity_name);
      $this->activateActivity($activateActivity, $this->getActivityStateHandler($activateActivity));
    }
    $activeActivities = &$this->activeActivities;
    unset($this->activeActivities[$activity->name]);
  }

  /**
   *
   */
  public function getActivityStateHandler(ConductorActivity $activity) {
    // TODO: Figure out what we need to do to get the Conductor Activity State handler.
    $activityState = new ConductorActivityState($activity, $this);
    foreach ($activity->inputs as $activityName) {
      if (isset($this->activityStates[$activityName])) {
        $activityState->setInputActivityState($this->activityStates[$activityName]);
      }
    }
    return $activityState;
  }

  public function workflowComplete() {
    $this->status == self::FINISHED;
  }

}
