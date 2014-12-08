/**
* @file
* Sets the summary for Workbench Scheduler on vertical tabs.
*/
(function ($) {

    // Hiding start and end date fields
    Drupal.behaviors.workbenchSchedulerOptions = {
        attach: function (context) {

            var type_input = $('input[name="workbench_scheduler_sid"]:checked');
            var start_date = $('input[name="workbench_scheduler_start_date[date]"]');
            var start_time = $('input[name="workbench_scheduler_start_date[time]"]');
            var end_date = $('input[name="workbench_scheduler_end_date[date]"]');
            var end_time = $('input[name="workbench_scheduler_end_date[time]"]');

            // Hiding start date field
            if (Drupal.settings.workbench_scheduler.schedules[type_input.val()].start_state == '') {
                start_date.val('');
                start_time.val('');
                $(".form-item-workbench-scheduler-start-date").hide();
            }
            else {
                $(".form-item-workbench-scheduler-start-date").show();
            }

            // Hiding end date field
            if (Drupal.settings.workbench_scheduler.schedules[type_input.val()].end_state == '') {
                end_date.val('');
                end_time.val('');
                $(".form-item-workbench-scheduler-end-date").hide();
            }
            else {
                $(".form-item-workbench-scheduler-end-date").show();
            }

            // This works but if there a more elegant way to do it. . .
            $("input[name=workbench_scheduler_sid]").change(function () {
                Drupal.behaviors.workbenchSchedulerOptions.attach(context);
            });
        }
    };

    // Vertical tabs
    Drupal.behaviors.workbenchSchedulerSettingsSummary = {
        attach: function (context) {

            $('#edit-workbench-scheduler', context).drupalSetSummary(function (context) {
                var vals = new Array();

                // Schedule type
                var type_input = $('input[name="workbench_scheduler_sid"]:checked');
                var type = Drupal.settings.workbench_scheduler.schedules[type_input.val()];

                vals.push(type.label);

                // If schedule exists
                if(type_input.val() > 0){

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
                }

               return vals.join("<br />");

            });
        }
    };

})(jQuery);

