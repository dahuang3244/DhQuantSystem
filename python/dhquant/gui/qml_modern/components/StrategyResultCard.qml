pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts

Rectangle {
    id: card
    radius: 16
    color: "#12191f"
    border.color: "#253038"
    Layout.fillWidth: true
    Layout.preferredHeight: 340

    property string strategyName: ""
    property real initialCapital: 10000
    property real finalCapital: 10519
    property real returnPct: 5.19
    property int tradeCount: 1
    property real winRate: 100.0
    property real maxDrawdown: 0.53
    property real deepDrawdown: 0.53
    property real avgHoldDays: 2.0
    property int maxHoldDays: 2
    property var tradesPnl: []
    property var equityCurve: []
    property var benchmarkCurve: []

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10

        // Strategy name + trade analysis button
        RowLayout {
            Layout.fillWidth: true
            Text { text: card.strategyName; color: "#5a9cf5"; font.pixelSize: 15; font.bold: true }
            Item { Layout.fillWidth: true }
            Rectangle {
                width: 100; height: 30; radius: 8
                color: "#1e2d3d"; border.color: "#2a3a4a"
                Text { anchors.centerIn: parent; text: "交易分析图 ▼"; color: "#8b99a6"; font.pixelSize: 10 }
            }
        }

        // Metrics row
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 46
            radius: 10; color: "#0f151a"; border.color: "#1e2a31"
            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 14; anchors.rightMargin: 14; spacing: 0
                Repeater {
                    model: [
                        {l:"初始资金", v:"$"+card.initialCapital.toLocaleString(), c:"#f3f5f7"},
                        {l:"最终资金", v:"$"+card.finalCapital.toLocaleString(), c: card.returnPct>=0?"#45c4a1":"#ed6b5f"},
                        {l:"收益率", v:(card.returnPct>=0?"+":"")+card.returnPct.toFixed(2)+"%", c: card.returnPct>=0?"#45c4a1":"#ed6b5f"},
                        {l:"交易次数", v:card.tradeCount.toString(), c:"#f3f5f7"},
                        {l:"胜率", v:card.winRate.toFixed(1)+"%", c: card.winRate>=50?"#45c4a1":"#ed6b5f"},
                        {l:"最大回撤", v:card.maxDrawdown.toFixed(2)+"%", c:"#d9a557"},
                        {l:"最深浮亏", v:card.deepDrawdown.toFixed(2)+"%", c:"#ed6b5f"},
                        {l:"平均持仓", v:card.avgHoldDays.toFixed(1)+"天", c:"#8b99a6"},
                        {l:"最长持仓", v:card.maxHoldDays+"天", c:"#8b99a6"}
                    ]
                    delegate: Item {
                        required property var modelData
                        Layout.fillWidth: true; Layout.fillHeight: true
                        Column {
                            anchors.verticalCenter: parent.verticalCenter; spacing: 2
                            Text { text: modelData.l; color: "#62707d"; font.pixelSize: 9 }
                            Text { text: modelData.v; color: modelData.c; font.pixelSize: 12; font.bold: true; font.family: "Menlo" }
                        }
                    }
                }
            }
        }

        // Charts row
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 12

            // Trade PnL bar chart
            Rectangle {
                Layout.preferredWidth: parent.width * 0.38
                Layout.fillHeight: true
                radius: 12; color: "#0f151a"; border.color: "#1e2a31"; clip: true

                Text { x: 14; y: 10; text: "交易盈亏"; color: "#8b99a6"; font.pixelSize: 12; font.bold: true }

                Canvas {
                    anchors.fill: parent
                    anchors.topMargin: 32; anchors.bottomMargin: 24
                    anchors.leftMargin: 14; anchors.rightMargin: 14
                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.reset(); ctx.clearRect(0,0,width,height);
                        var data = card.tradesPnl;
                        if(!data||data.length===0) return;
                        var maxV=0;
                        for(var i=0;i<data.length;i++) if(Math.abs(data[i])>maxV) maxV=Math.abs(data[i]);
                        if(maxV<0.01) maxV=1;
                        var n=data.length, bw=Math.max(4,Math.min(30,(width-20)/n*0.7));
                        var gap=(width-n*bw)/(n+1);
                        var mid=height/2;
                        ctx.strokeStyle="#1e2a31"; ctx.lineWidth=1;
                        ctx.beginPath(); ctx.moveTo(0,mid); ctx.lineTo(width,mid); ctx.stroke();
                        for(var j=0;j<n;j++){
                            var x=gap+(bw+gap)*j;
                            var h=Math.abs(data[j])/maxV*(height/2-8);
                            var y=data[j]>=0?mid-h:mid;
                            ctx.fillStyle=data[j]>=0?"#45c4a1":"#ed6b5f";
                            ctx.beginPath();
                            ctx.roundRect(x,y,bw,h,3);
                            ctx.fill();
                        }
                        ctx.fillStyle="#62707d"; ctx.font="9px Menlo"; ctx.textAlign="left";
                        for(var k=0;k<n;k++){
                            var lx=gap+(bw+gap)*k+bw/2;
                            ctx.textAlign="center";
                            ctx.fillText("#"+(k+1),lx,height-2);
                        }
                    }
                }
            }

            // Equity curve chart
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 12; color: "#0f151a"; border.color: "#1e2a31"; clip: true

                RowLayout {
                    x: 14; y: 8; spacing: 16
                    Text { text: "资产走势"; color: "#8b99a6"; font.pixelSize: 12; font.bold: true }
                    Row { spacing: 4; Rectangle{width:10;height:3;radius:1;color:"#5a9cf5";anchors.verticalCenter:parent.verticalCenter} Text{text:"策略交易";color:"#62707d";font.pixelSize:9} }
                    Row { spacing: 4; Rectangle{width:10;height:3;radius:1;color:"#45c4a1";anchors.verticalCenter:parent.verticalCenter} Text{text:"股票走势";color:"#62707d";font.pixelSize:9} }
                }

                Canvas {
                    anchors.fill: parent
                    anchors.topMargin: 32; anchors.bottomMargin: 24
                    anchors.leftMargin: 44; anchors.rightMargin: 14
                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.reset(); ctx.clearRect(0,0,width,height);
                        var eq = card.equityCurve, bm = card.benchmarkCurve;
                        if(!eq||eq.length<2) return;
                        var allMin=Infinity, allMax=-Infinity;
                        for(var i=0;i<eq.length;i++){
                            if(eq[i]<allMin)allMin=eq[i]; if(eq[i]>allMax)allMax=eq[i];
                            if(bm&&i<bm.length){ if(bm[i]<allMin)allMin=bm[i]; if(bm[i]>allMax)allMax=bm[i]; }
                        }
                        var spread=allMax-allMin; if(spread<1)spread=1;
                        allMin-=spread*0.05; allMax+=spread*0.05; spread=allMax-allMin;
                        // Grid
                        ctx.strokeStyle="#1e2a31"; ctx.lineWidth=0.5;
                        for(var g=0;g<=4;g++){
                            var gy=height*g/4;
                            ctx.beginPath(); ctx.moveTo(0,gy); ctx.lineTo(width,gy); ctx.stroke();
                            ctx.fillStyle="#62707d"; ctx.font="9px Menlo"; ctx.textAlign="right";
                            var gv=allMax-spread*g/4;
                            ctx.fillText("$"+Math.round(gv).toLocaleString(),-6,gy+3);
                        }
                        function drawLine(data,color,lw){
                            ctx.beginPath(); ctx.strokeStyle=color; ctx.lineWidth=lw;
                            for(var k=0;k<data.length;k++){
                                var px=width*k/(data.length-1);
                                var py=height*(1-(data[k]-allMin)/spread);
                                if(k===0)ctx.moveTo(px,py); else ctx.lineTo(px,py);
                            }
                            ctx.stroke();
                        }
                        if(bm&&bm.length>0) drawLine(bm,"#45c4a1",1.5);
                        drawLine(eq,"#5a9cf5",2);
                    }
                }
            }
        }
    }
}
