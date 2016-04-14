$(function() {

$('#socialbuttons .facebook_like').socialbutton('facebook_like', {
	button:'box_count',
	url:'http://www.html5-memo.com'
}).width(80);

$('#socialbuttons .twitter').socialbutton('twitter', {
	button:'vertical',
	text: '推文文字',
	url:'http://www.html5-memo.com',
	lang:'zh_cn',
	via:'html5memo'
}).width(80);


$('#socialbuttons .google').socialbutton('google_plusone', {
	 size:'tall',
	url:'http://www.html5-memo.com',
	count:true
}).width(60);


$('#socialbuttons .hatena').socialbutton('hatena', {
    button:'vertical',
    url:'http://www.html5-memo.com',
    encoded:true,
}).width(60);


$('#socialbuttons .pinterest').socialbutton('pinterest', {
	button:'vertical',
	media: 'http://cruzin.jp/zoo/img/03.jpg',
	url:'http://cruzin.jp/zoo/',
	description: 'Photo by Yutaka Hayashi'
}).width(60);

});