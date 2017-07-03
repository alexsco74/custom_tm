<?php

/**
 * @file
 * custom_tm class.
 */
class customTm {
  public $filters = array();
  public $actions = array();
  public $content = array();

  function __construct() {

    // filters
    $vocabularies = taxonomy_get_vocabularies();

    $voc_options = array('' => t('- Select -'));

    $pid_options = array();
    if (!empty($vocabularies)) {
      foreach ($vocabularies as $vid => $voc) {
        $voc_options[$vid] = $voc->name;
      }
    }

    $filters = array(
      'vid' => array(
        '#type' => 'select',
        '#title' => t('From vocabulary'),
        '#options' => $voc_options,
        '#required' => TRUE,
        '#attributes' => array(
          'required' => TRUE,
          'class' => array('custom-tm-auto-submit-item')
        ),
        '#table_alias' => 'ttd',
      ),
      'parent' => array(
        '#type' => 'radios',
        '#title' => t('From parent'),
        '#options' => $pid_options,
        '#attributes' => array(
          'class' => array(
            'custom-tm-auto-submit-item',
            'custom-tm-big-radios',
          ),
        ),
        '#table_alias' => 'tth',
      ),
    );


    foreach ($filters as $filter_key => $filter_data) {
      $this->filters[$filter_key] = $filter_data;
    }

    //content
    $select = db_select('taxonomy_term_data', 'ttd')->fields('ttd');
    $select->innerJoin('taxonomy_vocabulary', 'v', 'v.vid = ttd.vid');
    $select->addField('v', 'name', 'v_name');
    $select->addField('v', 'weight', 'v_weight');
    $select->orderBy('v.weight', 'ASC');
    $select->orderBy('v.name', 'ASC');

    //parent
    $select->innerJoin('taxonomy_term_hierarchy', 'tth', 'tth.tid = ttd.tid');

    foreach ($this->filters as $filter_key => &$filter_element) {
      if (!empty($_GET[$filter_key])) {
        $filter_value_xss = trim(filter_xss($_GET[$filter_key], array()));
        if (!empty($filter_value_xss) && $filter_value_xss != '_none') {
          $select->condition($filter_element['#table_alias'] . '.' . $filter_key, $filter_value_xss);
          $filter_element['#default_value'] = $filter_value_xss;
        }
      }
    }

    $select->orderBy('ttd.weight', 'ASC');
    $select->orderBy('ttd.name', 'ASC');

    $result = $select->execute();

    if (!empty($result)) {
      foreach ($result as $term) {
        $this->content[$term->tid] = $term;
      }
    }

    if (!empty($this->filters['vid']['#default_value'])) {
      $tree = taxonomy_get_tree($this->filters['vid']['#default_value']);


      if (!empty($tree)) {

        //get parent_options
        $parent_query = db_select('taxonomy_term_hierarchy', 'tth')->fields('tth', array('parent'));
        $parent_query->innerJoin('taxonomy_term_data', 'ttd', 'ttd.tid = tth.tid AND ttd.vid = :vid', array(':vid' => $this->filters['vid']['#default_value']));
        $parents = $parent_query->execute()->fetchCol();


        $temp_content = $this->content;
        $this->content = array();

        foreach ($tree as $tree_term) {

          if (in_array($tree_term->tid, $parents, TRUE)) {
            $option_title = str_repeat("&nbsp;", $tree_term->depth) . $tree_term->name;
            if ($tree_term->depth == 0) {
              $option_title = '<b>' . $option_title . '</b>';
            }
            elseif ($tree_term->depth == 1) {
              $option_title = '<i>' . $option_title . '</i>';
            }
            $this->filters['parent']['#options'][$tree_term->tid] = $option_title;
          }

          if (isset($temp_content[$tree_term->tid])) {
            $this->content[$tree_term->tid] = $temp_content[$tree_term->tid];
            $this->content[$tree_term->tid]->depth = $tree_term->depth;
            $this->content[$tree_term->tid]->parents = $tree_term->parents;
          }
        }

      }
    }

    // actions
    $actions_options = $voc_options;
    $actions = array(
      'move' => array(
        '#type' => 'select',
        '#title' => t('In vocabulary'),
        '#options' => $actions_options,
        '#required' => TRUE,
        '#attributes' => array(
          'required' => TRUE,
          'class' => array('custom-tm-auto-submit-item'),
        ),
      ),
      'move_parent' => array(
        '#type' => 'radios',
        '#title' => t('In parent'),
        '#options' => array(),
        '#attributes' => array('class' => array('custom-tm-big-radios')),
      ),
    );


    foreach ($actions as $action_key => $action_data) {
      $this->actions[$action_key] = $action_data;
    }

    //set default value
    foreach ($this->actions as $action_key => &$action_element) {
      if (!empty($_GET[$action_key])) {
        $action_value_xss = trim(filter_xss($_GET[$action_key], array()));
        if (!empty($action_value_xss) && $action_value_xss != '_none') {
          $action_element['#default_value'] = $action_value_xss;
        }
      }
    }

    //set move parent options
    if (!empty($this->actions['move']['#default_value'])) {
      $move = $this->actions['move']['#default_value'];
      $move_parent_tree = taxonomy_get_tree($move);

      if (!empty($move_parent_tree)) {

        //get move_parents
        foreach ($move_parent_tree as $tree_term) {
          $option_title = str_repeat("&nbsp;&nbsp;&nbsp;", $tree_term->depth) . $tree_term->name;
          if ($tree_term->depth == 0) {
            $option_title = '<b>' . $option_title . '</b>';
          }
          elseif ($tree_term->depth == 1) {
            $option_title = '<i>' . $option_title . '</i>';
          }
          $this->actions['move_parent']['#options'][$tree_term->tid] = $option_title;

        }
      }
    }
  }

  public function getQuery($form_state_values) {

    $query = $_GET;
    unset($query['q']);

    foreach ($this->filters as $filer_key => $filter) {
      if (isset($form_state_values[$filer_key])) {
        $filter_value_xss = trim(filter_xss($form_state_values[$filer_key], array()));
        if (!empty($filter_value_xss) && $filter_value_xss != '_none') {
          $query[$filer_key] = $filter_value_xss;
        }
        else {
          unset($query[$filer_key]);
        }
      }
    }

    foreach ($this->actions as $filer_key => $filter) {
      if (isset($form_state_values[$filer_key])) {
        $filter_value_xss = trim(filter_xss($form_state_values[$filer_key], array()));
        if (!empty($filter_value_xss) && $filter_value_xss != '_none') {
          $query[$filer_key] = $filter_value_xss;
        }
        else {
          unset($query[$filer_key]);
        }
      }
    }

    return $query;
  }

  public function buildHtml() {
    $build['wr'] = array(
      '#prefix' => '<div class="custom-tm-page-wr">',
      '#suffix' => '</div>',
    );
    $build['wr']['title'] = array(
      '#markup' => '<div class="custom-tm-description-wr">
<div>Здесь можно переместить термины из одного словаря в другой.</div>
<div><span class="custom-tm-attention">ВНИМАНИЕ!</span> При переносе терминов с разной структурой полей словаря, возможна потеря информации по термину.</div>
<div>Перед переносом рекомендуется <a href="/admin/config/system/backup_migrate">сделать архив базы!<a></div>
</div>'
    );
    $build['wr']['filters'] = drupal_get_form('custom_tm_filters_form', $this->filters);
    $build['wr']['actions'] = drupal_get_form('custom_tm_actions_form', $this->actions, $this->content, $this->filters);
    return $build;
  }
}