'use strict';

function submitForm() {
    var $alertBox = $('#alertBox');
    $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for domain creation.')).slideDown();
    $('#domainURL').val(replaceURL($('#domainURL').val()));
    
  var submitThis = true;
  var inputs = [
    'domainTitle',
    'domainURL',
  ];

  var data = {};
  inputs.forEach(function(value) {
    switch (checkFormElement('#' + value)) {
      case true:
        data[value] = getInputValue('#' + value);
        break;

      case false:
        submitThis = false;
        break;
      default:
        // Skip value
        break;
    }
  });

  if (submitThis) {
    $.ajax({
      type: "POST",
      url: '/ajax/domains/domain/processNewDomain',
      data: {
        data: data
      }
    })
      .done(function(response) {
        if (typeof(response['status']) != 'undefined') {
          if (typeof(response['domainID']) != 'undefined') {
            if (response['domainID']) {
              window.location.href = '/domains/' + response['domainID'];
            } else {
              // Tell user error
              $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
            }
          }
        } else {
            $alertBox.html(bootstrapAlert('warning', 'The details you entered are not valid, please try again.')).slideDown();
        }
      })
      .fail(function() {
        // Tell user error
        $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
      });
  } else {
    $alertBox.html(bootstrapAlert('warning', 'The details you entered are not valid, please try again.')).slideDown();
  }
}

function updateForm() {
    var $alertBox = $('#alertBox');
    $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for domain update.')).slideDown();
    $('#domainURL').val(replaceURL($('#domainURL').val()));
    
  var submitThis = false;
  var inputs = [
      'domainTitle',
      'domainURL',
  ];

  var data = {};
  inputs.forEach(function(value) {
    switch (checkFormElement('#' + value)) {
      case true:
        data[value] = getInputValue('#' + value);
        submitThis = true;
        break;
      default:
        // Skip value
        break;
    }
  });

  if (submitThis) {
    // Append domain ID
    data['domainID'] = $('#domainID').val();
      
    $.ajax({
      type: "POST",
      url: '/ajax/domains/domain/processUpdateDomain',
      data: {
        data: data
      }
    })
      .done(function(response) {
        if (typeof(response.status) != 'undefined') {
            $alertBox.html(bootstrapAlert('success', 'Domain successfully edited.')).slideDown();
        } else {
            $alertBox.html(bootstrapAlert('warning', 'The details you entered are not valid, please try again.')).slideDown();
        }
      })
      .fail(function() {
        // Tell user error
        $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
      });
  } else {
    $alertBox.html(bootstrapAlert('warning', 'The details you entered are not valid, please try again.')).slideDown();
  }
}

function deleteDomain() {
    var $alertBox = $('#alertBox');
    $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for domain deletion.')).slideDown();
    var data = {'domainStatus': 0};

    // Append domain ID
    data['domainID'] = $('#domainID').val();

    $.ajax({
        type: "POST",
        url: '/ajax/domains/domain/processUpdateDomain',
        data: {
            data: data
        }
    })
    .done(function(response) {
        if (typeof(response.status) != 'undefined') {
            $alertBox.html(bootstrapAlert('success', 'Domain successfully deleted.')).slideDown();
        } else {
            $alertBox.html(bootstrapAlert('warning', 'The details you entered are not valid, please try again.')).slideDown();
        }
    })
    .fail(function() {
        // Tell user error
        $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
    });
}
