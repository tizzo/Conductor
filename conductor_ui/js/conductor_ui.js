(function ($) {

Drupal.Conductor = Drupal.Conductor || {};
Drupal.Conductor.activities = Drupal.Conductor.activities || {};

Drupal.behaviors.conductor_ui = {
  attach: function (context, settings) {
    // Grab local convenience versions of global vars.
    var activities = Drupal.settings.conductor_ui.activities;
    var workflow = Drupal.Conductor.workflow;
    // Iterate over each activity activating it and popuplating it with whatever
    // information we have.
    var activity = null;
    for (i in activities) {
      // Identify the js representation to use for this particular
      // activity plugin.
      //console.log(activities[i]);
      var jsActivityObject = activities[i].plugin_info.conductur_ui_js_object;
      activity = $('.' + activities[i]['css_class'], context).each(function() { Drupal.Conductor.activities[jsActivityObject](this, activities[i]) });
      // Respond to events issued by the activity
      $(activity).live({
        release: function(e, data) {
          // For now, the data about this element is stashed via jQuery.
          // $(data.activityDomElement).data('activityInfo')
        }
      });
      //workflow.activities[i] = activity;
      //workflow.drawLines();
    }

    jsPlumb.draggable($('.conductor-ui-activity-activity-1'));
    jsPlumb.connect({
      source: "conductor-ui-activity-activity-1",
      target: "conductor-ui-activity-activity-2",
      //connector:"Flowchart",
      endpointsOnTop:true,
      paintStyle:{
          lineWidth:9,
          strokeStyle: "#CCC",
          outlineColor:"#666",
          outlineWidth:1
      },
/*
      anchors:["Center", "Center"],
      endpointStyle:{ radius:95, fillStyle: "#ccc"},
      labelStyle : { cssClass:"component label" },
      label : "big\nendpoints"
*/
    });
  }
}

/**
 *  A javascript representation of a Conductor Workflow.
 */
Drupal.Conductor.workflow = function(){
  var activities = {};
  /**
   *
   */
  var drawLines = function drawlines() {
    alert('monkey');
    return FALSE;
    for (i in activities) {
      console.log(activities);
    }
  }
}

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
  $(activityDomElement).draggable(
    {
      stack: "#conductor-workflow-editor div",
      // Ideally we'd like to keep these boxes inside their parent
      // but allow the parent to grown downward.  The line below
      // does not do that.
      //containment: '#conductor-workflow-editor',
      stop: function(event, ui) {
        x = ui.position['left'];
        y = ui.position['top']
        var e = $.Event("release");
        $(activityDomElement).trigger(e, {activityDomElement: activityDomElement});
      }
    }
  );
  $('.conductor-ui-activity-link', activityDomElement).click(function(e) {
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


