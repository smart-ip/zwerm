<?php
/**
 * User: tanguycoenen
 * Date: 11/09/12
 * Time: 10:50
 */

$links = $variables['links'];
$attributes = $variables['attributes'];
$heading = $variables['heading'];
global $language_url;
$output = '';

if (count($links) > 0) {
    $output = '';
    $output .= '<ul' . drupal_attributes($attributes) . '>';

    $num_links = count($links);
    $i = 1;

    foreach ($links as $key => $link) {
        $class = array($key);

        // Add first, last and active classes to the list of links to help out themers.
        if ($i == 1) {
            $class[] = 'first';
        }
        if ($i == $num_links) {
            $class[] = 'last';
        }
        if (isset($link['href']) && ($link['href'] == $_GET['q'] || ($link['href'] == '<front>' && drupal_is_front_page()))
            && (empty($link['language']) || $link['language']->language == $language_url->language)) {
            $class[] = 'active';
        }
        if (drupal_is_front_page())
        {
            $output .= '<li' . drupal_attributes(array('class' => $class)) . ' onmousedown="li_mousedown(\' ' . $link['href'] . '\',this)">';
        }
        else
        {
            $output .= '<li' . drupal_attributes(array('class' => $class)) . '>';
        }
        $output .= '<div  id="'.$link['title'].'" class="menu_item">'; // for hooking up the correct images in the css to the nav items
        if (isset($link['href'])) {
            // Pass in $link as $options, they share the same keys.
            $output .= l($link['title'], $link['href'], $link);
        }
        elseif (!empty($link['title'])) {
            // Some links are actually not links, but we wrap these in <span> for adding title and class attributes.
            if (empty($link['html'])) {
                $link['title'] = check_plain($link['title']);
            }
            $span_attributes = '';
            if (isset($link['attributes'])) {
                $span_attributes = drupal_attributes($link['attributes']);
            }
            $output .= '<span' . $span_attributes . '>' . $link['title'] . '</span>';
        }

        $i++;
        $output .= "</div>\n";
        $output .= "</li>\n";

    }

    $output .= '</ul>';
}
print $output;
?>