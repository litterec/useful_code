var isset=function ()       {var a=arguments, l=a.length, i=0;if (l===0) {    throw new Error('Empty isset');}while (i!==l) {    if (typeof(a[i])=='undefined' || a[i]===null) {        return false;    } else {        i++;    }}return true;        }  // The end of issetfunction hide_windows()       {jQuery('#lgn_form').hide();document.getElementById("lgn_form").style.display="none";//remove_ajax_handlers();return false;       }  // The end of hide_windowsfunction remove_ajax_handlers()       {//    alert("remove_ajax_handlers");jQuery(document).unbind("ajaxComplete");jQuery(document).unbind("ajaxSend");       }  // The end of remove_ajax_handlers       jQuery(document).ready(function($){$('.close-wnd').click(function() {//        document.getElementById("lgn_frm").style.display="none";//        alert('cross is pressed');    $('#lgn_frm').hide();});$('#lgn_frm').click(function(event) {    var senderElementName = event.target.id.toLowerCase();     if(senderElementName === 'lgn_frm'){            $(this).hide();     };//        alert('cross is pressed');});$('#sgnin_wnd').click(function() {//            alert('jQuery is pressed');    $('#lgn_frm').show();});       });  // The end of JQuery.ready