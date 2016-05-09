/**
 * @file
 *
 * The javascript functionality for workbench Scheduler.
 * Sets the summary for Workbench Scheduler on vertical tabs.
 */

(function ($) {

    // Select all table checkbox. 
    Drupal.behaviors.workbenchSchedulerSettingsSelectAllToggle = {
       attach: function (context) {
           // Trigger change event on all checkboxes. 
           $('table.workbench-scheduler-sid th.select-all > input.form-checkbox').change(function() {
               $('input.workbench-scheduler-sid').change();
           });
       } 
    }
    // Vertical tabs.
    Drupal.behaviors.workbenchSchedulerSettingsSummary = {
        attach: function (context) {

            $('#edit-workbench-scheduler', context).drupalSetSummary(function (context) {
                var vals = [];

                // Schedule type.
                $('input.workbench-scheduler-sid:checked').each(function(){
                    var type_input = $(this);
                    var type = Drupal.settings.workbench_scheduler.schedules[type_input.val()];

                    // If schedule exists.
                    if (type_input.val() > 0) {

                        // Date and Time.
                        var date = $('input[name="workbench_scheduler_date[' + type_input.val() + '][date]"]');
                        var time = $('input[name="workbench_scheduler_date[' + type_input.val() + '][time]"]');

                        // Reformat date to be easier to understand
                        if (date.val() || time.val()) {
                            vals.push(type.label + ': ' + Drupal.t('@date @time', {
                                    '@date': date.val(),
                                    '@time': time.val()
                                }));
                        }
                    }
                });

                return vals.join("<br />");
            });
        }
    };

})(jQuery);
