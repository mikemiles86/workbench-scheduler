/**
* @file
* Sets the summary for Workbench Scheduler on vertical tabs.
*/
(function ($) {

    // Vertical tabs
    Drupal.behaviors.workbenchSchedulerSettingsSummary = {
        attach: function (context) {

            $('#edit-workbench-scheduler', context).drupalSetSummary(function (context) {
                var vals = [];

                // Schedule type
                var type_input = $('input[name="workbench_scheduler_sid"]:checked');
                var type = Drupal.settings.workbench_scheduler.schedules[type_input.val()];
                vals.push(type.label);

                // Start Date and Time
                var start_date = $('input[name="workbench_scheduler_start_date[date]"]');
                var start_time = $('input[name="workbench_scheduler_start_date[time]"]');
                if (start_date.val() || start_time.val()) {
                    vals.push(Drupal.checkPlain('Start Date: ' + start_date.val() + ' ' + start_time.val()));
                }

                // End Date and Time
                var end_date = $('input[name="workbench_scheduler_end_date[date]"]');
                var end_time = $('input[name="workbench_scheduler_end_date[time]"]');
                if (end_date.val() || end_time.val()) {
                    vals.push(Drupal.checkPlain('End Date: ' + end_date.val() + ' ' + end_time.val()));
                }

               return vals.join("<br />");

            });
        }
    };

})(jQuery);
