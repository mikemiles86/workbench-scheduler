<?php

/**
 * @file
 * Provides admin functions for Workbench Scheduler.
 */


/**
 * display a table of workbench schedule for administration
 * @return string  themed output.
 */
function workbench_scheduler_admin_page() {
  //build a table to show the different schedules
  $headers = array(
    array('data' => t('Name')),
    array('data' => t('Machine Name')),
    array('data' => t('Start State')),
    array('data' => t('End State')),
    array('data' => t('Content Types')),
    array('data' => t('Operations'), 'colspan' => 2),
  );

  $rows = array();
  //retrieve any schedules that exist.
  if ($schedules = workbench_scheduler_load_schedules()) {
    //get list of the different moderation states
    $states = workbench_scheduler_state_labels();
    //get list of the different content types.
    $node_types = node_type_get_types();
    //loop through the schedules to add them to the table
    foreach ($schedules as $name => $schedule) {
      //format the content types the schedule is available for
      //based on number
      $type_count = sizeof($schedule->types);
      //more then one type?
      if ($type_count > 1) {
        $items = array();
        //loop through each type
        foreach ($schedule->types as $type) {
          //display the human readable name
          $items[] = $node_types[$type]->name;
        }

        //format into an item list
        $types = theme('item_list', array('items' => $items, 'type' => 'ul'));
      // have only a single type?
      }
      elseif (sizeof($schedule->types) == 1) {
        //display the human readable name
        $types = $node_types[array_pop($schedule->types)]->name;
      //no types found (either deleted or not imported?)
      }
      else {
        //display null.
        $types = 'NULL';
      }
      //format the row
      $row = array(
        $schedule->label,
        $name,
        $states[$schedule->start_state],
        $states[$schedule->end_state],
        $types,
        //link to edit the schedule
        l(t('Edit'), 'admin/config/workbench/scheduler/schedules/' . $name . '/edit'),
        //link to delete the scheduler
        l(t('Delete'), 'admin/config/workbench/scheduler/schedules/' . $name . '/delete'),
      );
      //add to the rows array
      $rows[] = $row;
    }
  //no schedules found
  }
  else {
    //display message in first row
    $rows[] = array(array(
        'data'    => t('No Schedules Found'),
        'colspan' => 7,
    ));
  }
  //add a row for a link to add a new schedule
  $rows[] = array(
    array(
      'data'    => l(t('Add Schedule'), 'admin/config/workbench/scheduler/schedules/add'),
      'colspan' => 7,
    )
  );
  //returned the themed table
  return theme('table', array('header' => $headers, 'rows' => $rows));

}

/**
 * display a form for adding/editing a schedule
 * @param  array      $form       the form array
 * @param  array      $form_state the form state array
 * @param bool|string $name       machine name of schedule (optional)
 * @return array              the form array
 * @ingroup forms
 */
function workbench_scheduler_admin_edit_schedule($form, &$form_state, $name=FALSE) {
  $schedule = FALSE;
  //passed a machine name? editing a schedule.
  if ($name) {
    //attempt to load the schedule
    if ($schedule =workbench_scheduler_load_schedules($name)) {
      //store in the form
      $form['#schedule']  = $schedule;
    }
  }
  //label field
  $form['label'] = array(
    '#type'           => 'textfield',
    '#title'          => t('Schedule Name'),
    '#required'       => TRUE,
    '#description'    => t('A user friendly name. Only used for admin purposes'),
    '#size'           => 40,
    '#maxlength'      => 127,
  );

  //field for machine name
  $form['name'] = array(
    '#title'          => t('Machine Name'),
    '#description'    => t('A machine friendly name.'),
    '#type'           => 'machine_name',
    '#required'       => TRUE,
    '#size'           => 25,
    '#maxlength'      => 25,
    '#default_value'  => ($schedule ? $schedule->name:''),
    '#machine_name'   => array(
      'exists' => '_workbench_schedule_check_machine_name_exists',
      'source' => array('label'),
    ),
  );
  //fetch a list of the available moderation states
  $states = workbench_scheduler_state_labels();
  //save to the form
  $form['#states'] = $states;

  //select list for start state
  $form['start_state'] = array(
    '#type'           => 'select',
    '#title'          => t('Start State'),
    '#description'    => t('Select the state to be set when a node reaches its \'start date\''),
    '#options'        => $states,
    '#required'       => TRUE,
  );

  //select list for end state
  $form['end_state'] = array(
    '#type'         => 'select',
    '#title'        => t('End State'),
    '#description'  => t('Select the state to be set when a node reaches its \'end date\''),
    '#options'      => $states,
    '#required'     => TRUE,
  );

  // fetch a list of content types that have moderation enabled.
  // from the workbench moderation module
  module_load_include('module', 'workbench_moderation');
  $types = workbench_moderation_moderate_node_types();
  $tmp_types = array();
  //make into an associative array
  foreach ($types as $type) {
    $tmp_types[$type] = $type;
  }
  $types = $tmp_types;
  unset($tmp_types);
  //store types to the form
  $form['#types'] = $types;

  //checkboxes for content types
  $form['types'] = array(
    '#type'           => 'checkboxes',
    '#title'          => t('Content Types'),
    '#description'    => t('Select the content types that can use this schedule'),
    '#options'        => $types,
    '#required'       => TRUE,
  );

  $form['submit'] = array(
    '#type'   => 'submit',
    '#value' => ($schedule ? t('Update'):t('Save')),
  );

  //editing a schedule?
  if ($schedule) {
    //add a delete button
    $form['delete'] = array(
      '#type'   => 'submit',
      '#value'  => t('Delete'),
    );
    //remove the machine name field
    unset($form['name']);
    //set default values for the other fields
    $form['label']['#default_value']        = $schedule->label;
    $form['start_state']['#default_value']  = $schedule->start_state;
    $form['end_state']['#default_value']    = $schedule->end_state;
    $form['types']['#default_value']        = $schedule->types;
  }
  //return the form
  return $form;
}

/**
 * validation function for adding / editing a schedule
 * @param  array $form       the form array
 * @param  array $form_state the form state array
 * @return none
 */
function workbench_scheduler_admin_edit_schedule_validate($form, &$form_state) {
  //validate label
  if (strlen(trim(strip_tags($form_state['values']['label'])))<1) {
    form_set_error('label', t('Invalid Schedule name provided'));
  }
  //check that the two states are not the same
  if ($form_state['values']['start_state'] == $form_state['values']['end_state']) {
    form_set_error('end_state', t('End state must be different from start state'));
  }
}

/**
 * submit function for adding/editing a schedule
 * @param  array $form       the form array
 * @param  array $form_state the form state array
 * @return none
 */
function workbench_scheduler_admin_edit_schedule_submit($form, &$form_state) {
  //check to see if the delete button was clicked
  if ($form_state['clicked_button']['#id'] == 'edit-delete') {
    //redirect to the delete form
    drupal_goto('admin/config/workbench/scheduler/schedules/' . $form['#schedule']->name . '/delete');
  //only other button is the submit button
  }
  else {
    //build the array of schedule data
    $schedule_data = array(
      'label'       => trim(strip_tags($form_state['values']['label'])),
      'start_state' => $form_state['values']['start_state'],
      'end_state'   => $form_state['values']['end_state'],
      //add the checked types to the schedule data
      'types'       => array_intersect($form['#types'], $form_state['values']['types']),
    );

    //updating an existing schedule?
    if (isset($form['#schedule'])) {
      //fetch the machine name form the object
      $machine_name = $form['#schedule']->name;
    }
    else {
      //fetch the new machine name from passed values
      $machine_name = $form_state['values']['name'];
    }

    //attempt a save/update
    if ($result = workbench_scheduler_save_schedule($machine_name, $schedule_data)) {
      drupal_set_message(t('Schedule ' . ($result==1 ? 'Saved':'Updated')), 'status', FALSE);
      //redirect back to the schedules table
      $form_state['redirect'] = 'admin/config/workbench/scheduler/schedules';
    }
    else {
      drupal_set_message(t('Error saving schedule'), 'error', FALSE);
    }
  }
}

/**
 * confirmation form for deleting a schedule
 * @param  array $form       the form array
 * @param  array $form_state the form state array
 * @param  string $name       the machine name of the schedule
 * @return none
 * @ingroup forms
 */
function workbench_scheduler_admin_delete_schedule($form, &$form_state, $name) {
  //attempt to load the schedule
  if ($schedule = workbench_scheduler_load_schedules($name)) {
    //store to form
    $form['#schedule'] = $schedule;
    //build confirmation form
    return confirm_form($form,
      t('Are you sure you want to delete the schedule "' . $schedule->label . '"?'),
      'admin/config/workbench/scheduler/schedules',
      t('This action cannot be undone')
    );
  }
  //unable to load form, not sure what trying to delete.
  else {
    drupal_set_message(t('Invalid Schedule machine name'), 'error', FALSE);
    //send back to schedules table
    drupal_goto('admin/config/workbench/scheduler/schedules');
  }
}

/**
 * submit function for deleting a schedule
 * @param  array $form       the form array
 * @param  array $form_state the form state array
 * @return none
 */
function workbench_scheduler_admin_delete_schedule_submit($form, &$form_state) {
  //fetch the schedule to delete
  $schedule = $form['#schedule'];
  //attempt to delete
  if (workbench_scheduler_delete_schedules($schedule->name)) {
    drupal_set_message(t('Schedule and associated data deleted'), 'status', FALSE);
    //go back to schedules page
    $form_state['redirect'] = 'admin/config/workbench/scheduler/schedules';
  }
  //unable to delete, show error message.
  else {
    drupal_set_message(t('Error deleting schedule'), 'error', FALSE);
  }
}
