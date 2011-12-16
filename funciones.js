
$(document).ready(function(){
$('li:has(ul)',".dinamic_infomenu")
.click(function(event){
if (this == event.target) {
if ($(this).children().is(':hidden')) {
$(this)
.css('list-style-image','url(minus.gif)')
.children().show();
} else {
$(this)
.css('list-style-image','url(plus.gif)')
.children('ul').hide();
}
}
return true;
})
.css('cursor','pointer')
.click();
$('li:not(:has(ul))').css({
cursor: 'default',
'list-style-image':'none'
});
});


