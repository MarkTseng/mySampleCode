;(function (d, $) {

	var jQdm_graph;

	jQdm_graph = function(){

    //
    // sample1_1
	    // 橫軸、縱軸標籤與項目圖標、網格背景顯示的折線圖
    //

	    var sample1_1_setting = { // Sample1的基本參數

      type : "line",
      margins: [20, 40, 40, 40],　// [10,10,10,10] -> [50,50,50,50]　設定圖形margin
      labels: // 橫軸Label
      ["2013-06","2013-07","2013-08","2013-09","2013-10","2013-11"],
      values: { // 各圖的值
        serie1 : [42.68,43.12,42.78,40.8,40.44,41.87],
        serie2 : [25.44,24.53,25.55,28.56,28.96,27.31],
        serie3 : [20.01,20.09,19.25,18.36,18.11,18.15],
        serie4 : [8.39,8.59,8.57,8.52,8.54,8.5],
        serie5 : [1.03,1.1,1.16,1.16,1.2,1.18]
      },
      series: { // 各圖的顏色
        serie1 : {
          color : "#f1c40f"
        },
        serie2 : {
          color : "#e67e22"
        },
        serie3 : {
          color : "#e74c3c"
        },
        serie4 : {
          color : "#27ae60"
        },
        serie5 : {
          color : "#2980b9"
        }
      },
      legend: { // 各圖的圖標
        serie1 : 'Chrome',
        serie2 : 'IE',
        serie3 : 'Firefox',
        serie4 : 'Safari',
        serie5 : 'Opeara'
      },

      defaultSeries: { // 圖形設定

          plotProps: { // 設定圖形外觀
              "stroke-width": 2 //　line圖形的線寬
        },

          legend: { // 圖標內容的樣式設定
              textProps: { // 圖標字形的樣式
            fill: "#999"
          }
        }

      },

      defaultAxis: { // defaultAxis(軸)的設定

          max: 50, // x -> 50  値的最大値
          labels: true, // false -> true 設定是否顯示軸的label
          labelsDistance: 15, // 8 label間的距離（px）
          labelsProps: { // label樣式
              font: '10px Arial',
              fill: "#000"
          },
          labelsAnchor: "auto", // auto 設定軸label的anchor點(以labelDistance、labelRotate為基礎點)
          //auto:軸label的中央
          //start:軸label的右側
          //end:軸label的左側

          labelsMargin: 0, // 0 軸label的margin，設定縱軸上方margin與橫軸左側margin

      },

      features: {　// 設定部分的設定

          grid: { // grid的設定
              nx: "auto", // auto 橫軸label顯示數
              ny: 6, // 4 縱軸label顯示數
              draw: true,　// false -> true 是否顯示grid
              props: { // grid樣式
                  stroke: '#dadada', // #e0e0e0
                  "stroke-width": 1 // 1
              },
              extra: [0, 0, 10, 0] // [0,0,0,0]　格線的超出寬度[上, 右, 下, 左]
        },

          legend: {　// 圖標額外設定
              x: 60, // 'auto' 圖標的x作標，當horizontal為true時，auto有效
              y: 10,
              width: 100, // horizontal為true時，auto有效
              height: 100,
              itemWidth: "fixed",
              // fixed 圖標各項目的寬度，fixed為固定。
              margins: [0, 0, 0, 0],
              dotMargins: [10, 5], // sx, dx
              borderProps: {
                  fill: "rgba( 255, 255, 255, 0.5)",
                  stroke: "black",
                  "stroke-width": 0
              },
              // 下面defaultSeries設定為優先
          dotType : 'rect',
          dotWidth : 10, dotHeight : 10, dotR : 4,
          dotProps : {
            type : "rect",
            width : 10,
            height : 10,
            "stroke-width" : 0
          },
          textProps : { font: '12px Arial', fill: "#000" }
        }
      },

      axis: {
          l: {
          suffix: '%'
        }
      }

    };

    $("#chart1_1").chart(sample1_1_setting);

    $.elycharts.templates['template_sample1_1'] =　sample1_1_setting;


    //
    // sample1_2
	    // 填滿區域並以圓點表示數值的折線圖
    //

    var sample1_2_setting = {

        template: 'template_sample1_1', // 載入第1個範例的樣版，繼承相關設定

        defaultSeries: { // 圖形設定(額外部分)

        fill: true,
        fillProps: { // 填滿效果的樣式
          stroke: "none",
          "stroke-width" : 0,
          "stroke-opacity": 0,
          opacity: .5
        },

        dot: true,  // false 是否以圓點顯示
        dotProps : {
          size: 4,
          stroke: "#000",
          zindex: 5
        }

      }

    };

	    // 製作sample1_2的圖
    $("#chart1_2").chart(sample1_2_setting);


    //
    // sample1_3
    //

    var sample1_3_setting = {

        template: 'template_sample1_1', // 載入第1個範例內容的選項，繼承相關設定

        tooltips: { // 各圖形的tooltip
        serie1 : ["42.68%","43.12%","42.78%","40.8%","40.44%","41.87%"],
        serie2 : ["25.44%","24.53%","25.55%","28.56%","28.96%","27.31%"],
        serie3 : ["20.01%","20.09%","19.25%","18.36%","18.11%","18.15%"],
        serie4 : ["8.39%","8.59%","8.57%","8.52%","8.54%","8.5%"],
        serie5 : ["1.03%","1.1%","1.16%","1.16%","1.2%","1.18%"]
      },

        defaultSeries: { // 圖形設定(額外部分)

        fill: true,
        fillProps: { // 填滿效果的樣式
          stroke: "none",
          "stroke-width" : 0,
          "stroke-opacity": 0,
          opacity: .5
        },

        dot: true,  // false 是否以圓點顯示
        dotProps : {
          size: 4,
          stroke: "#000",
          zindex: 5
        },

        tooltip: { //　圖形tooltip的設定
            active: true,  // true 是否支援tooltip？
            width: 80, // tooltip寬度
            height: 40, //　tooltip高度
            roundedCorners: 5,  // 設定tooltip圓角（px）
            padding: [10, 10], // [6, 6] tooltip padding，同CSS依照[縱、橫]的順序
            offset: [10, 10], // [20, 0] tooltip margin，同CSS依照[縱、橫]的順序
            frameProps: {  // tooltip顯示
                fill: "rgba( 0, 0, 0, 0.7)",  // white 背景色
                "stroke-width": 4 // 2 框線顏色
            },
            contentStyle: {  // tooltip的內容樣式
                "font-family": "Arial", // font
                "font-size": "16px",  // 12px font size
                "line-height": "24px",  // 16px 行高
                color: "#fff"  // black 文字顏色
            }
        },

        highlight: { // 滑鼠移到圖形時的動作
            scale: [3, 3], // [x,y]->[2,1] highlight時的放大率
          scaleSpeed : 100, scaleEasing : '',
          newProps : {
            opacity : 1
          },
          overlayProps : {
            "fill" : "white",
            "fill-opacity" : .3,
            "stroke-width" : 0
          },
          move : 0,
          moveSpeed : 100, moveEasing : '',
          restoreSpeed : 0, restoreEasing : ''
        }

      }

    };

    $("#chart1_3").chart(sample1_3_setting);

	}

	jQdm_graph();

})(document, jQuery);