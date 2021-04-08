import QtQuick 2.12
import QtQuick.Shapes 1.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import IzLibrary 1.0
import IzTreeView 1.0

IzTreeViewController {
	id: root

	// used to draw route from node to parentLimesInferior66
	function traceRoute(node) {
		if (node.hParent) {
			var pNode = root.visualNodeFromID(node.pnID);
			if (pNode !== null) {
				var c = Qt.createComponent("qrc:/include/IzTreeView/QML/LineComponent.qml");
				var tracePath = c.createObject(root.shapeCanvas, {
												   "sourceNode": node,
												   "parentNode": root.visualNodeFromID(node.pnID),
												   "strokeWidth": 2,
												   "spacing": Qt.binding(function(){
													   return root.columnSpacing;
												   }),
												   "strokeColor": Qt.binding(function(){
													   return node.v ? Material.color(Material.Blue) : "transparent";
												   })
											   });

				root.traceCanvas.data.push(tracePath);
				root.traceRoute(root.visualNodeFromID(root.getTreeData(node.treeIndex, "parentNodeID")));
			} else {
				var cNoParent = Qt.createComponent("qrc:/include/IzTreeView/QML/NoParentLineComponent.qml");
				var tracePathNoParent = cNoParent.createObject(root.shapeCanvas, {
												   "sourceNode": node,
												   "strokeWidth": 2,
												   "spacing": Qt.binding(function(){
													   return root.columnSpacing;
												   }),
												   "strokeColor": Qt.binding(function(){
													   return node.v ? Material.color(Material.Blue) : "transparent";
												   })
											   });

				root.traceCanvas.data.push(tracePathNoParent);
			}
		}
	}

	// used to clear tree view data
	function clearData() {
		root.clearNodes();
	}

	// returns node width for given nodeSize
	function calculateNodeHeight() {
		switch(root.nodeSize) {
		case IzTreeNodeSize.Small:
			return 25;
		case IzTreeNodeSize.Medium:
			return 30;
		case IzTreeNodeSize.Large:
			return 40;
		case IzTreeNodeSize.Custom:
			return root.nodeHeight;
		}
	}

	// returns node height for given nodeSize
	function calculateNodeWidth() {
		switch(root.nodeSize) {
		case IzTreeNodeSize.Small:
			return 95;
		case IzTreeNodeSize.Medium:
			return 200;
		case IzTreeNodeSize.Large:
			return 250;
		case IzTreeNodeSize.Custom:
			return root.nodeWidth;
		}
	}

	property int nodeSize: IzTreeNodeSize.Medium
	property Shape traceCanvas: null
	property url nodeDelegateURL: ""

	property bool mouseInteractionEnabled: true
	property bool dragDropEnabled: false
	property bool nodeControlsEnabled: false
	property bool filtersEnabled: false
	property bool quickSettingsEnabled: false

	signal released(int index, QtObject mouse)
	signal nodeDropped(QtObject source, QtObject target)

	flickable: flickable
	nodeWidth: 95
	nodeHeight: 25
	columnSpacing: 10
	rowSpacing: 10

	implicitHeight: (root.treeLevel + 1) * root.nodeHeight + (root.rowSpacing * root.treeLevel)

	onLoadStarted: {
		flickable.prerefreshContentY = flickable.contentY;
	}

	onLoadEnded: {
		if (flickable.contentHeight > flickable.prerefreshContentY) {
			flickable.contentY = flickable.prerefreshContentY;
		}
	}

	onDataGenerated: {
		root.flickable.contentX = 0;
		root.flickable.contentY = 0;
	}

	onCurrentIndexChanged: {
		if (root.traceCanvas !== null) {
			root.traceCanvas.destroy();
		}

		if (currentNodeID !== -1) {
			var currentVisualNode = root.visualNodeFromID(currentNodeID);
			if (currentVisualNode !== null) {
				var tc = shapeComponent.createObject(flickable.contentItem);
				root.traceCanvas = tc;
				root.traceRoute(currentVisualNode);
			}
		}
	}

	onViewChanged: {
		if (root.traceCanvas !== null) {
			root.traceCanvas.destroy();
		}

		if (currentNodeID !== -1) {
			var currentVisualNode = root.visualNodeFromID(currentNodeID);
			if (currentVisualNode !== null) {
				var tc = shapeComponent.createObject(flickable.contentItem);
				root.traceCanvas = tc;
				root.traceRoute(currentVisualNode);
			}
		}
	}

	onDataCleared: {
		if (root.traceCanvas !== null) {
			root.traceCanvas.destroy();
		}

		root.currentIndex = -1;
		connectorsInstantiator.model.resetModel();
	}

	onInvalidRegExpSet: {
		IzToast.show(root, qsTr("Ustawione wyrażenie regularne jest niepoprawne."), "alert");
	}

	Component {
		id: shapeComponent

		Shape {
			z: flickable.z + 10
		}
	}

	QuickSettings {
		id: quickSettings

		width: 300
		anchors {
			left: parent.left
			bottom: parent.bottom
		}

		z: globalFilter.z + 1
		visible: root.quickSettingsEnabled
		customNodeHeight: root.nodeHeight
		customNodeWidth: root.nodeWidth
		customRowSpacing: root.rowSpacing
		customColumnSpacing: root.columnSpacing
		selectedNodeSize: root.nodeSize
		quickSettingsEnabled: root.quickSettingsEnabled

		onSelectedNodeSizeChanged: {
			root.nodeSize = selectedNodeSize;
			root.nodeHeight = root.calculateNodeHeight();
			root.nodeWidth = root.calculateNodeWidth();
		}

		onCustomNodeHeightChanged: {
			root.nodeHeight = customNodeHeight;
		}

		onCustomNodeWidthChanged: {
			root.nodeWidth = customNodeWidth;
		}

		onCustomRowSpacingChanged: {
			root.rowSpacing = customRowSpacing;
		}

		onCustomColumnSpacingChanged: {
			root.columnSpacing = customColumnSpacing;
		}
	}

	ProgressBar {
		anchors {
			left: parent.left
			right: parent.right
			bottom: parent.bottom
		}

		visible: root.isWorking
		indeterminate: true
	}

	ColumnLayout {
		id: mainLayout

		anchors.fill: parent
		spacing: 5

		RowLayout {
			Layout.fillWidth: true
			Layout.preferredHeight: 30

			visible: root.filtersEnabled

			IzButton {
				Layout.preferredHeight: 30
				Layout.preferredWidth: 30
				fontIcon: root.regExpFilteringEnabled ? "\uf451" : "\uf02c"
				tooltip: root.regExpFilteringEnabled ? qsTr("Filtrowanie RegExp") : qsTr("Filtrowanie proste")

				onReleased: {
					root.regExpFilteringEnabled = !root.regExpFilteringEnabled;
				}
			}

			IzTextField {
				id: globalFilter

				Layout.fillWidth: true
				Layout.preferredHeight: 30

				placeholderText: qsTr("Filtr danych...")
				enabled: root.hasData

				onEditingFinished: {
					if (filterDelayTimer.running) {
						filterDelayTimer.stop();
					}
					root.setGlobalFilter(globalFilter.text);
				}

				onTextChanged: {
					if (filterDelayTimer.running) {
						filterDelayTimer.restart();
					} else {
						filterDelayTimer.start();
					}
				}

				Timer {
					id: filterDelayTimer

					interval: 500

					onTriggered: {
						root.setGlobalFilter(globalFilter.text);
					}
				}

				ProgressBar {
					id: progressBar

					anchors {
						left: parent.left
						right: parent.right
						bottom: parent.bottom
					}
					height: 2

					indeterminate: true
					visible: root.isFilteringData

					Material.accent: Material.color(Material.Blue)
				}
			}
		}

		Flickable {
			id: flickable

			property real span : contentY + height
			property real prerefreshContentY: 0

			Layout.fillHeight: true
			Layout.fillWidth: true

			contentWidth: (root.treeDepth + 1) * root.calculateNodeWidth() + (root.columnSpacing * root.treeDepth)
			contentHeight: (root.treeLevel + 1) * root.calculateNodeHeight() + (root.rowSpacing * root.treeLevel)
			clip: true
			boundsBehavior: Flickable.StopAtBounds
			flickableDirection: Flickable.HorizontalAndVerticalFlick

			ScrollBar.vertical: ScrollBar {
				minimumSize: 0.1
				width: 5
			}

			ScrollBar.horizontal: ScrollBar {
				minimumSize: 0.1
				height: 5
			}

			Instantiator {
				id: connectorsInstantiator

				property int elements: 0

				model: root.connectorsModel
				asynchronous: true

				delegate: Rectangle {
					id: connector

					parent: flickable.contentItem

					width: root.calculateNodeWidth()
					height: root.calculateNodeHeight()

					x: Math.floor((width + root.columnSpacing) * parentX)
					y: Math.floor((height + root.rowSpacing) * parentY)
					z: parent.z + 1

					color: "transparent"
					border.width: 1
					border.color: Qt.darker(root.Material.accent)

					Shape {
						anchors.fill: parent

						vendorExtensionsEnabled: false

						ShapePath {
							id: shape

							strokeWidth: 1
							strokeColor: "#a5d6a7"
							fillColor: "transparent"

							startX: connector.width
							startY: connector.height / 2

							PathLine {
								x: connector.width + root.columnSpacing / 4
								y: connector.height / 2
							}

							PathLine {
								x: connector.width + root.columnSpacing / 4
								y: length * (root.rowSpacing + root.nodeHeight) + root.nodeHeight / 2
							}
						}
					}
				}
			}

			Instantiator {
				id: instantiator

				model: root.visualModel

				delegate: ItemDelegate {
					id: delegateRoot

					// TODO: zmienić nazwy
					property int treeIndex: index
					property int nID: nodeID
					property int pnID: parentNodeID
					property bool hParent: hasParent
					property bool v: isVisible
					property Shape shape: null

					width: root.calculateNodeWidth()
					height: root.calculateNodeHeight()
					x: Math.floor((width + root.columnSpacing) * nodeDepth)
					y: Math.floor((height + root.rowSpacing) * nodeLevel)
					z: parent.z + 2

					padding: 0
					topPadding: 0
					bottomPadding: 0
					topInset: 0
					bottomInset: 0
					visible: y + height > flickable.contentY && y < flickable.span && isVisible
					highlighted: nodeID === root.currentNodeID
					parent: flickable.contentItem

					contentItem: Rectangle {
						id: contentRoot

						border.width: 1
						border.color: Material.color(Material.Grey, Material.Shade700)
						radius: 2
						color: "transparent"

						DropArea {
							id: dropTarget

							property alias dropProxy: dropTarget

							anchors.fill: parent
							keys: [ "node" ]

							onDropped: {
								if (drop.source.sourceProxy !== delegateRoot) {
									root.nodeDropped(root.nodeByID(drop.source.sourceProxy.nID), root.nodeByID(delegateRoot.nID));
								}
							}
						}

						states: [
							State {
								when: mouseArea.drag.active

								PropertyChanges {
									target: contentRoot
									border.color: root.Material.accent
								}
							},

							State {
								when: delegateRoot.hovered

								PropertyChanges {
									target: contentRoot
									border.color: root.Material.accent
								}
							},

							State {
								when: dropTarget.containsDrag

								PropertyChanges {
									target: contentRoot
									border.color: Material.color(Material.Cyan)
								}
							},

							State {
								when: delegateRoot.highlighted

								PropertyChanges {
									target: contentRoot
									border.color: Material.color(Material.Blue)
								}
							}
						]

						MouseArea {
							id: mouseArea

							anchors.fill: parent
							hoverEnabled: false
							acceptedButtons: Qt.LeftButton | Qt.RightButton
							propagateComposedEvents: true
							drag.target: dragIndicator

							onReleased: {
								if(drag.active) {
									dragIndicator.Drag.drop();
									return;
								}

								if (!root.mouseInteractionEnabled) {
									return;
								}

								root.currentIndex = delegateRoot.treeIndex;
								root.released(root.currentIndex, mouse);
							}

							Loader {
								id: nodeContent

								anchors {
									fill: parent
									margins: 1
								}

								source: root.nodeDelegateURL.toString() === ""
										? "qrc:/include/IzTreeView/QML/DefaultNodeDelegate.qml"
										: root.nodeDelegateURL

								Rectangle {
									id: dragIndicator

									property alias sourceProxy: delegateRoot

									width: mouseArea.drag.active ? nodeContent.width : 15
									height: mouseArea.drag.active ? nodeContent.height : 3

									visible: root.dragDropEnabled
											 && delegateRoot.hParent
									color: mouseArea.drag.active ? "transparent" : root.Material.accent
									border.color: mouseArea.drag.active ?  root.Material.accent : "transparent"
									border.width: mouseArea.drag.active ? 1 : 0
									radius: mouseArea.drag.active ? 2 : 0

									Drag.keys: [ "node" ]
									Drag.active: mouseArea.drag.active
												 && delegateRoot.hParent
									Drag.hotSpot.x: 0
									Drag.hotSpot.y: 0

									Behavior on width {
										NumberAnimation {
											duration: 85
										}
									}

									Behavior on height {
										NumberAnimation {
											duration: 85
										}
									}

									Behavior on x {
										NumberAnimation {
											duration: 85
										}
									}

									Behavior on y {
										NumberAnimation {
											duration: 85
										}
									}

									states: State {
										when: !mouseArea.drag.active

										PropertyChanges {
											target: dragIndicator
											x: 0
											y: 0
										}
									}
								}

								IzButton {
									id: visibilityButton

									width: 20
									height: 20
									anchors {
										bottom: parent.bottom
										bottomMargin: -4
										right: parent.right
										rightMargin: -3
									}

									visible: root.nodeControlsEnabled ? (hasChildren ? true : false) : false
									enabled: root.nodeControlsEnabled ? (hasChildren ? true : false) : false
									fontIcon: childrenHidden ? "\uf35e" : "\uf35d"
									tooltip: childrenHidden ? qsTr("Rozwiń poziom") : qsTr("Zwiń poziom")

									onReleased: {
										root.changeNodeVisibility(delegateRoot.treeIndex, !childrenHidden);
									}
								}
							}
						}
					}
				}

				onObjectAdded: {
					if (object.hParent) {
						var lineComponent = Qt.createComponent("qrc:/include/IzTreeView/QML/LineShapeComponent.qml");
						object.shape = lineComponent.createObject(flickable.contentItem, {
																   "node": object,
																   "spacing": Qt.binding(function() {
																	   return root.columnSpacing;
																   })
															   });
						if (lineComponent.status === Component.Error) {
							console.log("Error loading component:", lineComponent.errorString());
						}
					}
					root.visualNodeCreated(object.nID, object);
				}

				onObjectRemoved: {
					// WARNING: podejrzany hack
					if (object !== null) {
						if (object.shape !== null) {
							object.shape.destroy();
						}
						root.visualNodeDestroyed(object.nID);
					}
				}
			}
		}
	}
}
