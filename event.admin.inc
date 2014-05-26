<?php

/**
 * @file
 * Event type editing UI.
 */

/**
 * UI controller.
 */
class EventTypeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage events and event types.';
    return $items;
  }
}

/**
 * Page callback: Generates the appropriate event administration form.
 *
 * This function generates the event, multiple event cancellation,
 * or filtered event list admin form, depending on the argument and the POST
 * form values.
 *
 * @param string $callback_arg
 *   (optional) Indicates which form to build. Defaults to '', which will
 *   trigger the user filter form. If the POST value 'op' is present, this
 *   function uses that value as the callback argument.
 *
 * @return string
 *   A renderable form array for the respective request.
 */
function event_admin($callback_arg = '') {
  $op = isset($_POST['op']) ? $_POST['op'] : $callback_arg;

  switch ($op) {
    case t('Add new event'):
    case 'add':
      $build['event_form'] = drupal_get_form('event_form');
      break;
    default:
      if (!empty($_POST['events']) && isset($_POST['operation']) && ($_POST['operation'] == 'cancel')) {
        $build['event_multiple_cancel_confirm'] = drupal_get_form('event_multiple_cancel_confirm');
      }
      else {
        $build['event_filter_form'] = drupal_get_form('event_filter_form');
        $build['event_admin_account'] = drupal_get_form('event_admin_account');
      }
  }
  return $build;
}
 
/**
 * Form builder; Return form for event administration filters.
 *
 * @ingroup forms
 * @see event_filter_form_submit()
 */
function event_filter_form($form, &$form_state) {
    
  $form['filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Show only events where'),
    '#theme' => 'exposed_filters__event',
  );
  
  $form['filters']['type'] = array(
    '#type' => 'select',
    '#title' => 'Event Type',
    '#options' => event_types(),
    '#default_value' => 'any',
    '#required' => FALSE,
  );
  
  $form['filters']['coordinator'] = array(
    '#type' => 'textfield',
    '#title' => 'Event Coordinator',
    '#size' => 60,
    '#autocomplete_path' => 'user/autocomplete',
    '#required' => FALSE,
  );
  
  $form['filters']['stime'] = array(
    '#type' => 'textfield',
    '#title' => 'Start Date',
    '#size' => 60, 
    '#required' => FALSE,
  );

  $form['filters']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
    '#weight' => 40,
  );
  
  return $form;
}

function event_filter_form_submit($form, &$form_state) {
    $form_state['redirect'] = 'admin/events';
  return;
    
}

/**
 * Process result from user administration filter form.
 
function user_filter_form_submit($form, &$form_state) {
  $op = $form_state['values']['op'];
  $filters = user_filters();
  switch ($op) {
    case t('Filter'):
    case t('Refine'):
      // Apply every filter that has a choice selected other than 'any'.
      foreach ($filters as $filter => $options) {
        if (isset($form_state['values'][$filter]) && $form_state['values'][$filter] != '[any]') {
          // Merge an array of arrays into one if necessary.
          $options = ($filter == 'permission') ? form_options_flatten($filters[$filter]['options']) : $filters[$filter]['options'];
          // Only accept valid selections offered on the dropdown, block bad input.
          if (isset($options[$form_state['values'][$filter]])) {
            $_SESSION['user_overview_filter'][] = array($filter, $form_state['values'][$filter]);
          }
        }
      }
      break;
    case t('Undo'):
      array_pop($_SESSION['user_overview_filter']);
      break;
    case t('Reset'):
      $_SESSION['user_overview_filter'] = array();
      break;
    case t('Update'):
      return;
  }

  $form_state['redirect'] = 'admin/people';
  return;
}*/

/**
 * Form builder; User administration page.
 *
 * @ingroup forms
 * @see user_admin_account_validate()
 * @see user_admin_account_submit()
 */
function event_admin_account() {
    

  $header = array(
    'title' => array('data' => t('Event Title'), 'field' => 'e.title'),
    'type' => array('data' => t('Type'), 'field' => 'e.type'),
    'coordinator' => array('data' => t('Coordinator'), 'field' => 'e.coordinator'),
    'start_date' => array('data' => t('Start Date'), 'field' => 'e.stime', 'sort' => 'desc'),
    'operations' => array('data' => t('Operations')),
  );
  
  $destination = drupal_get_destination();
  
  $result = db_select('event', 'e');
  
  foreach ($result as $event) {
    
    $options[$event->event_id] = array(
      'title' => theme('title', array('title' => $event)),
      'type' =>  theme('type', array('type' => $event)),
      'coordinator' => theme('coordinator', array('coordinator' => $event)),
      'start_date' => format_interval(REQUEST_TIME - $event->stime),
      'operations' => array('data' => array('#type' => 'link', '#title' => t('edit'), '#href' => "event/$event->event_id/edit", '#options' => array('query' => $destination))),
    );
  }
  
  
  
  $form['events'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No events available.'),
  );
  
  return $form;
  
/* 
  $header = array(
    'username' => array('data' => t('Username'), 'field' => 'u.name'),
    'status' => array('data' => t('Status'), 'field' => 'u.status'),
    'roles' => array('data' => t('Roles')),
    'member_for' => array('data' => t('Member for'), 'field' => 'u.created', 'sort' => 'desc'),
    'access' => array('data' => t('Last access'), 'field' => 'u.access'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('users', 'u');
  $query->condition('u.uid', 0, '<>');
  user_build_filter_query($query);

  $count_query = clone $query;
  $count_query->addExpression('COUNT(u.uid)');

  $query = $query->extend('PagerDefault')->extend('TableSort');
  $query
    ->fields('u', array('uid', 'name', 'status', 'created', 'access'))
    ->limit(50)
    ->orderByHeader($header)
    ->setCountQuery($count_query);
  $result = $query->execute();

  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $options = array();
  foreach (module_invoke_all('user_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => $options,
    '#default_value' => 'unblock',
  );
  $options = array();
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );

  $destination = drupal_get_destination();

  $status = array(t('blocked'), t('active'));
  $roles = array_map('check_plain', user_roles(TRUE));
  $accounts = array();
  foreach ($result as $account) {
    $users_roles = array();
    $roles_result = db_query('SELECT rid FROM {users_roles} WHERE uid = :uid', array(':uid' => $account->uid));
    foreach ($roles_result as $user_role) {
      $users_roles[] = $roles[$user_role->rid];
    }
    asort($users_roles);

    $options[$account->uid] = array(
      'username' => theme('username', array('account' => $account)),
      'status' =>  $status[$account->status],
      'roles' => theme('item_list', array('items' => $users_roles)),
      'member_for' => format_interval(REQUEST_TIME - $account->created),
      'access' =>  $account->access ? t('@time ago', array('@time' => format_interval(REQUEST_TIME - $account->access))) : t('never'),
      'operations' => array('data' => array('#type' => 'link', '#title' => t('edit'), '#href' => "user/$account->uid/edit", '#options' => array('query' => $destination))),
    );
  }

  $form['accounts'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No people available.'),
  );
  $form['pager'] = array('#markup' => theme('pager'));

  return $form;*/
}

/**
 * Generates the event type editing form.
 */
function event_type_form($form, &$form_state, $event_type, $op = 'edit') {

  if ($op == 'clone') {
    $event_type->label .= ' (cloned)';
    $event_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $event_type->label,
    '#description' => t('The human-readable name of this event type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($event_type->type) ? $event_type->type : '',
    '#maxlength' => 32,
    '#disabled' => $event_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'event_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this event type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['weight'] = array(
    '#type' => 'textfield',
    '#size' => 10,
    '#maxlength' => 10,
    '#default_value' => 0,
    '#description' => t('Weight of the event type.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save event type'),
    '#weight' => 40,
  );

  if (!$event_type->isLocked() && $op != 'add' && $op != 'clone') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete event type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('event_type_form_submit_delete')
    );
  }
  return $form;
}

/**
 * Submit handler for creating/editing event type.
 */
function event_type_form_submit(&$form, &$form_state) {
  $event_type = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  event_type_save($event_type);

  // Redirect user back to list of event types.
  $form_state['redirect'] = 'admin/events';
}

function event_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/event-types/' . $form_state['event_type']->type . '/delete';
}

/**
 * Event type delete form.
 */
function event_type_form_delete_confirm($form, &$form_state, $event_type) {
  $form_state['event_type'] = $event_type;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['event_type_id'] = array('#type' => 'value', '#value' => entity_id('event_type' ,$event_type));
  return confirm_form($form,
    t('Are you sure you want to delete event type %title?', array('%title' => entity_label('event_type', $event_type))),
    'event/' . entity_id('event_type' ,$event_type),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Event type delete form submit handler.
 */
function event_type_form_delete_confirm_submit($form, &$form_state) {
  $event_type = $form_state['event_type'];
  event_type_delete($event_type);

  watchdog('event_type', '@type: deleted %title.', array('@type' => $event_type->type, '%title' => $event_type->label));
  drupal_set_message(t('@type %title has been deleted.', array('@type' => $event_type->type, '%title' => $event_type->label)));

  $form_state['redirect'] = 'admin/structure/event-types';
}

/**
 * Page to select event Type to add new event.
 */
function event_admin_add_page() {
  $items = array();
  foreach (event_types() as $event_type_key => $event_type) {
    $items[] = l(entity_label('event_type', $event_type), 'event/add/' . $event_type_key);
  }
  return array('list' => array('#theme' => 'item_list', '#items' => $items, '#title' => t('Select type of event to create.')));
}

/**
 * Add new event page callback.
 */
function event_add($type) {
  $event_type = event_types($type);

  $event = entity_create('event', array('type' => $type));
  drupal_set_title(t('Create @name', array('@name' => entity_label('event_type', $event_type))));

  $output = drupal_get_form('event_form', $type);

  return $output;
}

/**
 * Event Form.
 */
function event_form($form, &$form_state, $event) {
  $form_state['event'] = $event;

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $event->label,
    '#description' => t('The human-readable name of this event.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  
  $form['uid'] = array(
    '#type' => 'textfield',
    '#title' => t('Coordinator'),
    '#maxlength' => 60,
    '#size' => 30,
    '#description' => t('Select a coordinator by typing their username to get a list of matches.'),
    '#autocomplete_path' => 'user/autocomplete',
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Email'),
    '#description' => t('A description of the event.'),
    '#cols' => 60,
    '#resizable' => TRUE,
    '#rows' => 5,
    '#default_value' => $event->description,
  );
  
  $form['location_building'] = array(
    '#type' => 'textfield',
    '#title' => t('Location for this event'),
    '#size' => 60, 
    '#maxlength' => 128,
    '#required' => FALSE,
    '#default' => '',
  );

  field_attach_form('event', $event, $form, $form_state);

  $submit = array();
  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions'] = array(
    '#weight' => 100,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save event'),
    '#submit' => $submit + array('event_form_submit'),
  );

  // Show Delete button if we edit event.
  $event_id = entity_id('event' ,$event);
  if (!empty($event_id) && event_access('edit', $event)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('event_form_submit_delete'),
    );
  }

  $form['#validate'][] = 'event_form_validate';

  return $form;
}

function event_form_validate($form, &$form_state) {

}

/**
 * Task submit handler.
 */
function event_form_submit($form, &$form_state) {
  $event = $form_state['event'];

  entity_form_submit_build_entity('event', $event, $form, $form_state);

  event_save($event);

  $event_uri = entity_uri('event', $event);

  $form_state['redirect'] = $event_uri['path'];

  drupal_set_message(t('Event %title saved.', array('%title' => entity_label('event', $event))));
}

function event_form_submit_delete($form, &$form_state) {
  $event = $form_state['event'];
  $event_uri = entity_uri('event', $event);
  $form_state['redirect'] = $event_uri['path'] . '/delete';
}

/**
 * Delete confirmation form.
 */
function event_delete_form($form, &$form_state, $event) {
  $form_state['event'] = $event;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['event_type_id'] = array('#type' => 'value', '#value' => entity_id('event' ,$event));
  $event_uri = entity_uri('event', $event);
  return confirm_form($form,
    t('Are you sure you want to delete event %title?', array('%title' => entity_label('event', $event))),
    $event_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete form submit handler.
 */
function event_delete_form_submit($form, &$form_state) {
  $event = $form_state['event'];
  event_delete($event);

  drupal_set_message(t('Event %title deleted.', array('%title' => entity_label('event', $event))));

  $form_state['redirect'] = '<front>';
}
