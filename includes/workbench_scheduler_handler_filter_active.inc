<?php

/**
 * @file
 * Filter based on active or inactive.
 */

class workbench_scheduler_handler_filter_active extends views_handler_filter_boolean_operator {
  
  /**
   * Return filter options for active or inactive schedules.
   */
  function query() {
    // Getting all the latest revisions of all the nodes.
    $query = db_select('node_revision', 'node_revision')
      ->groupBy('node_revision.nid'); 
    $query->addExpression('MAX(node_revision.vid)', 'latest_vid');
    $query->join('workbench_scheduler_nodes', 'workbench_scheduler_nodes', 'workbench_scheduler_nodes.nid = node_revision.nid');
    $latest_vids = $query->execute()->fetchCol('latest_vid'); 

    // Filter by latest vids. 
    $operator = ($this->value ? "IN" : "NOT IN");
    $this->ensure_my_table();
    $this->query->add_where($this->options['group'], "$this->table_alias.vid", $latest_vids, $operator);
  }
}
