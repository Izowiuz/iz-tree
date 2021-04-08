import QtQuick 2.12
import QtQuick.Shapes 1.12

Shape {
	id: root

	property QtObject node: null
	property int spacing

	visible: root.node.v
	vendorExtensionsEnabled: false

	ShapePath {
		id: shape

		strokeWidth: 1
		strokeColor: "#a5d6a7"
		fillColor: "transparent"

		startX: root.node.x
		startY: root.node.y + root.node.height / 2

		PathLine {
			x: root.node.x - root.spacing + root.spacing / 4
			y: root.node.y + root.node.height / 2
		}
	}
}
