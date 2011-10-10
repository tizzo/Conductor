(function ($) {

Drupal.Conductor = Drupal.Conductor || {};
// TODO: What if someone wants to put two instances of the conductor interface
// on screen at the same time?  Can we make this more self contained?
Drupal.Conductor.activities = Drupal.Conductor.activities || {};
Drupal.Conductor.jsPlumbLoaded = false;
Drupal.Conductor.setupHasRun = false;

Drupal.behaviors.conductor_ui = {
  attach: function (context, settings) {
    // Wait for jsPlumb to become available.
    if (Drupal.Conductor.jsPlumbLoaded == false) {
      Drupal.Conductor.jsPlumbLoaded = true;
      jsPlumb.bind('ready', function() {
        jsPlumb.setAutomaticRepaint(true);
        Drupal.Conductor.setup(context, settings);
      });
    }
    else {
      Drupal.Conductor.setup(context, settings);
    }
  }
}
Drupal.Conductor.setup = function (context, settings) {
  if (Drupal.Conductor.setupHasRun != true) {
    // Grab local convenience versions of global vars.
    var activities = Drupal.settings.conductor_ui.activities;
    var workflow = Drupal.Conductor.workflow;

    // Iterate over each activity activating it and popuplating it with whatever
    // information we have.
    var activity = null;
    for (i in activities) {
      // Identify the js representation to use for this particular
      // activity plugin.
      var jsActivityObject = activities[i].plugin_info.conductur_ui_js_object;
      activity = $('.' + activities[i]['css_class'], context).each(function() { Drupal.Conductor.activities[jsActivityObject](this, activities[i]) });
      // Respond to events issued by the activity
      $(activity).live({
        release: function(e, data) {
          // For now, the data about this element is stashed via jQuery.
          // $(data.activityDomElement).data('activityInfo')
          // TODO: we're not properly getting the updated x and y coordinates on
          // drop.  This is related in some way to the draggable options for
          // jsPlumb and how they are passed through to jQuery ui.
        }
      });
      var info = activity.data('activityInfo');
      // Inform the workflow object of the activities that are members of it.
      workflow.activities[info.name] = {};
      // Stow the dom element for easy access.
      workflow.activities[info.name].domElement = activity;
      // Stow the js object for easy access.
      workflow.activities[info.name].activityInfo = info;
    }
    workflow.initLines();
    Drupal.Conductor.setupHasRun = true;
  }
}


/**
 *  A javascript representation of a Conductor Workflow.
 */
Drupal.Conductor.workflow = {
  activities: {},
  inputs: {},
  outputs: {},
  // Initialize the lines between activity nodes.
  initLines: function() {
    for (i in this.activities) {
      var info = this.activities[i].activityInfo;
      this.inputs[info.name] = info.inputs;
      this.outputs[info.name] = info.outputs;
    }
    // Make activities draggable
    // Loop through each activity for processing.
    for (activity in this.inputs) {
      // Loop through inputs associated with this activity.
      for (input in this.inputs[activity]) {
        if (this.outputs[this.inputs[activity][input]] != undefined) {
          if ($.inArray(activity, this.outputs[this.inputs[activity][input]]) != -1) {
            jsPlumb.connect({
              source: this.activities[this.inputs[activity][input]].activityInfo.css_class,
              target: this.activities[activity].activityInfo.css_class,
              endpointsOnTop:true,
              anchors:["AutoDefault", "AutoDefault"],
              // connector: "Straight",
              // TODO: Make this arrow look less terrible.
              overlays: [
                ["Arrow", {
                  location:0.9, width:30,
                }],
                /*
                ["Label", {
                  label: Drupal.t('Output'),
                  location:0.5, width:30,
                }],
                */
              ], 
              connector: [
                "Bezier",
                { curviness: 20 }
              ],
              paintStyle:{
                  lineWidth:9,
                  strokeStyle: "#CCC",
                  //outlineColor:"#666",
                  //outlineWidth:1
              },
              endpointStyle:{ radius:1, fillStyle: "#ccc"},
            });
          }
        }
      }
    }
  }
};


/**
 * A javascript representation of a base Conductor Activity.
 *
 * This is the base object that should be used by other activities as the
 * prototype to be extended.
 */
Drupal.Conductor.activities.activity = function(activityDomElement, activityInfo) {

  // Initialize member variables.
  var x = activityInfo['x'];
  var y = activityInfo['y'];
  var inputs = {};
  var outputs = {};

  // Place the element on the page.
  $(activityDomElement).css('top', y + 'px');
  $(activityDomElement).css('left', x + 'px');

  // Stash data inside this DOM element so that we can work with it later.
  $(activityDomElement).data('activityInfo', activityInfo);
  // Make activities draggable.
  var jQueryDraggableOptions = {
    stack: "#conductor-workflow-editor div",
    // Ideally we'd like to keep these boxes inside their parent
    // but allow the parent to grown downward.  The line below
    // does not do that.
    //containment: '#conductor-workflow-editor',
    stop: function(event, ui) {
      console.log(this.data('activityInfo'));
      x = ui.position['left'];
      y = ui.position['top']
      var e = $.Event("release");
      $(activityDomElement).trigger(e, {activityDomElement: activityDomElement});
    }
  }
  jsPlumb.draggable(activityDomElement, jQueryDraggableOptions);
  // TODO: there's a better way than singling out the edit button.  For shizzle.
  $('.conductor-ui-activity-link a:not(.edit)', activityDomElement).click(function(e) {
    e.preventDefault();
  });
}

/**
 * TODO: This should be moved into the plugin folder.
 *
 * It is currently here only for convenient testing.
 */
 // TODO: Better document the use of extend
 // Extend is used here to clone the parent object such that it can be added to
 // without modifying the original.
Drupal.Conductor.activities.activityConditional = $.extend(true, {}, Drupal.Conductor.activities.activity);
Drupal.Conductor.activities.activityConditional.something = function() {
  // TODO: flesh this out
}

})(jQuery);


