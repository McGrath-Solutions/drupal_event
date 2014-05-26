<?php

/**
 * Extend the defaults.
 */
class EventMetadataController extends EntityDefaultMetadataController {

  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    $properties['title'] = array(
      'label' => t('Title'),
      'description' => t('The event label.'),
      'setter callback' => 'entity_property_verbatim_set',
      'setter permission' => 'administer events',
      'schema field' => 'label',
    );

    $properties['type'] = array(
      'type' => 'text',
      'getter callback' => 'entity_property_getter_method',
      'setter callback' => 'entity_property_verbatim_set',
      'setter permission' => 'administer events',
      'required' => TRUE,
      'description' => t('The event type.'),
    ) + $properties['type'];

    unset($properties['uid']);

    $properties['created'] = array(
      'label' => t("Date created"),
      'type' => 'date',
      'description' => t("The date the event was created."),
      'setter callback' => 'entity_property_verbatim_set',
      'setter permission' => 'administer events',
      'schema field' => 'created',
    );
    $properties['changed'] = array(
      'label' => t("Date changed"),
      'type' => 'date',
      'schema field' => 'changed',
      'description' => t("The date the event was most recently updated."),
    );

    return $info;
  }
}