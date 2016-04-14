$(function() {

$('#socialbuttons .facebook_like').socialbutton('facebook_like', {
	button:'button_count',
	url:'http://www.html5-memo.com',
	locale:'zh_tw'
}).width(115);


$('#socialbuttons .twitter').socialbutton('twitter', {
	button:'horizontal',
	text: '推文文字',
	url:'http://www.html5-memo.com',
	lang:'zh_tw',
	via:'html5memo'
}).width(105);


$('#socialbuttons .google').socialbutton('google_plusone', {
	size:'medium',
	url:'http://www.html5-memo.com',
	count:true
}).width(80);


$('#socialbuttons .hatena').socialbutton('hatena', {
    button:'standard',
    url:'http://www.html5-memo.com',
}).width(60);


$('#socialbuttons .pinterest').socialbutton('pinterest', {
	button:'horizontal',
	media: 'http://cruzin.jp/zoo/img/03.jpg',
	url:'http://cruzin.jp/zoo/',
	description: 'Photo by Yutaka Hayashi'
}).width(100);

});