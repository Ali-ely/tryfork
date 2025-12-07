//
//  GraphBridge.h
//  TestMap
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface GraphBridge : NSObject

+ (BOOL)loadGraphFromBundleJSON:(NSString *)bundleFilename error:(NSError * _Nullable *)error;
+ (BOOL)loadGraphFromFilePath:(NSString *)filePath error:(NSError * _Nullable *)error;
+ (nullable NSString *)findNodeIdByName:(NSString *)name NS_SWIFT_NAME(findNodeId(byName:));
+ (nullable NSArray<NSString *> *)shortestPathFrom:(NSString *)nodeIdFrom to:(NSString *)nodeIdTo;
+ (nullable NSArray<NSDictionary *> *)geometryForPathNodeIds:(NSArray<NSString *> *)path;

@end

NS_ASSUME_NONNULL_END
