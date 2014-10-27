'use strict';

function submitForm() {
  var $alertBox = $('#alertBox');
  $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for server creation.')).slideDown();

  var submitThis = true;
  var inputs = [
    'serverTitle',
    'serverIP',
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
      url: '/ajax/servers/server/processNewServer',
      data: {
        data: data
      }
    })
      .done(function(response) {
        if (typeof(response['status']) != 'undefined') {
          if (typeof(response['serverID']) != 'undefined') {
            if (response['serverID']) {
              window.location.href = '/servers/' + response['serverID'];
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
  $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for server update.')).slideDown();

  var submitThis = false;
  var inputs = [
      'serverTitle',
      'serverIP',
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
    // Append server ID
    data['serverID'] = $('#serverID').val();
      
    $.ajax({
      type: "POST",
      url: '/ajax/servers/server/processUpdateServer',
      data: {
        data: data
      }
    })
      .done(function(response) {
        if (typeof(response.status) != 'undefined') {
            $alertBox.html(bootstrapAlert('success', 'Server successfully edited.')).slideDown();
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

function deleteServer() {
    var $alertBox = $('#alertBox');
    $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for server deletion.')).slideDown();
    var data = {'serverStatus': 0};

    // Append server ID
    data['serverID'] = $('#serverID').val();

    $.ajax({
        type: "POST",
        url: '/ajax/servers/server/processUpdateServer',
        data: {
            data: data
        }
    })
    .done(function(response) {
        if (typeof(response.status) != 'undefined') {
            $alertBox.html(bootstrapAlert('success', 'Server successfully deleted.')).slideDown();
        }
    })
    .fail(function() {
        // Tell user error
        $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
    });
}


function addDomain($this) {
    $('#domainModalSaveButton').attr('disabled', 'disabled');
    
    if($this) {

        var data = {'serverID':  $('#serverID').val(), 'domainID': $this.val()};
        $.ajax({
            type: "POST",
            url: '/ajax/servers/domain/processAddDomain',
            data: {
                data: data
            }
        }).done(function(response) {
            if (typeof(response.status) != 'undefined') {
                var row = '<tr><td>' + $this.val() + '</td>' +
                    '<td><a href="/domains/' + $this.val() + '">' + $this.attr('data-title') + '</a></td>' +
                    '<td><a href="http://' + $this.attr('data-url') + '">' + $this.attr('data-url') + '</a></td>' +
                    '<td><button type="button" class="btn btn-default removeDomain pull-right" data-id="' + $this.val() + '"><span class="glyphicons remove"></span></button></td>' +
                    '</tr>';

                $('#domainTableBody').append(row);
                $('#domainModal').modal('hide');
            }
        }).fail(function() {
            // Tell user error
            alert('Something went wrong, please try again');
            $('#domainModal').modal('hide');
        });
    } else {
        var data = {'serverID':  $('#serverID').val()};

        $.ajax({
            type: "POST",
            url: '/ajax/servers/domain/processAddDomain',
            data: {
                data: data
            }
        }).done(function(response) {
            if (typeof(response.status) != 'undefined') {
                if(response.result) {
                    var select = '<select id="addDomainModalSelect" class="form-control"><option selected="selected" disabled="disabled" value="">Please Select</option>';
                    
                    response.result.forEach(function(value){
                        select += '<option value="' + value['id'] + '" data-title="' + value['title'] + '" data-url="' +  value['url'] + '">' + value['title'] + '</option>';
                    });

                    select += '</select>';
                    
                    $('#domainModalBody').html(select);
                    $('#domainModalSaveButton').removeAttr('disabled');
                    $('#domainModalSaveButton').off('click');
                    $('#domainModalSaveButton').on('click', function(){
                        var $this = $('#addDomainModalSelect');
                        if($this.val()) {
                            addDomain($this.find(':selected'));
                        }
                    });
                    $('#domainModal').modal('show');
                }
            }
        }).fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
    }
    
}

function removeDomain($this) {
    var data = {'domainID': $this.attr('data-id'), 'serverID':  $('#serverID').val()};

    $.ajax({
        type: "POST",
        url: '/ajax/servers/domain/processRemoveDomain',
        data: {
            data: data
        }
    }).done(function(response) {
        if (typeof(response.status) != 'undefined') {
            $this.parent().parent().remove();
        }
    })
    .fail(function() {
        // Tell user error
        $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
    });
}

$(document).ready(function(){
   $(document).on('click', '.removeDomain', function(){
       removeDomain($(this));
   });
});