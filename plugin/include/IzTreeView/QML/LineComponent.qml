import QtQuick 2.12
import QtQuick.Shapes 1.12

ShapePath {
	id: root

	property QtObject sourceNode: null
	property QtObject parentNode: null
	property int spacing

	strokeWidth: 1
	strokeColor: (!root.sourceNode || !root.parentNode ) ? "transparent" : (root.sourceNode.isVisible ? "#a5d6a7" : "transparent")
	fillColor: "transparent"

	startX: root.parentNode && root.sourceNode ? (root.parentNode.x + root.sourceNode.width) : 0
	startY: root.parentNode && root.sourceNode ? (root.parentNode.y + root.sourceNode.height / 2) : 0

	PathLine {
		x: root.parentNode && root.sourceNode ? (root.parentNode.x + root.sourceNode.width + root.spacing / 4) : 0
		y: root.parentNode && root.sourceNode ? (root.parentNode.y + root.sourceNode.height / 2) : 0
	}

	PathLine {
		x: root.parentNode && root.sourceNode ? (root.parentNode.x + root.sourceNode.width + root.spacing / 4) : 0
		y: root.sourceNode ? (root.sourceNode.y + root.sourceNode.height / 2) : 0
	}

	PathLine {
		x: root.sourceNode ? (root.sourceNode.x) : 0
		y: root.sourceNode ? (root.sourceNode.y + root.sourceNode.height / 2) : 0
	}
}
